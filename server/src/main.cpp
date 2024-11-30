
#include <qglobal.h>
#include <qimage.h>
#include <qobject.h>
#include <qstringview.h>

#include <QApplication>
#include <QDataStream>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>

#include <absl/flags/flag.h>
#include <absl/flags/internal/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <absl/log/globals.h>
#include <absl/log/initialize.h>
#include <absl/time/clock.h>
#include <absl/time/time.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <grpcpp/support/status.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include "QualityStructure.h"
#include "camera.h"
#include "database/mysql.h"
#include "database/user.h"
#include "engine.h"
#include "face_database.h"
#include "grpc/include/rpc_server.h"
#include "gui.h"
#include "seeta/CFaceInfo.h"
#include "seeta/Common/CStruct.h"
#include "sync_queue/core.h"
#include "utils/base.h"

#include "face.pb.h"

using arm_face_id::MySqlSchema;
using arm_face_id::RecognitionRequest;
using arm_face_id::RecognitionResponse;
using arm_face_id::RegistrationRequest;
using arm_face_id::RegistrationResponse;

using fmt::format;

ABSL_FLAG(int, camera_index, 0, "native camera device`s index.");
ABSL_FLAG(std::string, camera_url, " ", "network camera`s url.");
ABSL_FLAG(std::string, server_addr, "127.0.0.1:25575", "gRPC server`s url.");
ABSL_FLAG(bool, gui, false, "enable the gui.");
ABSL_FLAG(std::string, model, "sf3.0_models", "the path of models.");

ABSL_FLAG(std::string, db_ip, "127.0.0.1", "the database IP address.");
ABSL_FLAG(int, db_port, 3306, "the database connection port.");
ABSL_FLAG(std::string, db_username, "root", "the username of database.");
ABSL_FLAG(std::string, db_password, "password", "the password of database.");
ABSL_FLAG(std::string, db_name, "db_armfaceid", "the name of database.");

int main(int argc, char* argv[]) {
  absl::SetProgramUsageMessage("availiable flags:");
  absl::ParseCommandLine(argc, argv);
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] [%s:%#] %v");
  SPDLOG_DEBUG("Initialized logger.");

  std::string db_ip = absl::GetFlag(FLAGS_db_ip);
  int db_port = absl::GetFlag(FLAGS_db_port);
  std::string db_username = absl::GetFlag(FLAGS_db_username);
  std::string db_password = absl::GetFlag(FLAGS_db_password);
  std::string db_name = absl::GetFlag(FLAGS_db_name);

  MySqlSchema mysql(db_ip, db_port, db_name, db_username, db_password);

  std::string model_path = absl::GetFlag(FLAGS_model);
  seeta::ModelSetting FD_model_setting(
      format("{}/face_detector.csta", model_path),
      SeetaDevice::SEETA_DEVICE_GPU);
  seeta::ModelSetting FR_model_setting(
      format("{}/face_recognizer.csta", model_path),
      SeetaDevice::SEETA_DEVICE_GPU);
  seeta::ModelSetting PD_model_setting(
      format("{}/face_landmarker_pts5.csta", model_path),
      SeetaDevice::SEETA_DEVICE_GPU);

  arm_face_id::FaceEngine::Settings engine_settings;
  engine_settings.fd_setting = FD_model_setting;
  engine_settings.fr_setting = FR_model_setting;
  engine_settings.pd_setting = PD_model_setting;

  auto engine = std::make_shared<arm_face_id::FaceEngine>(engine_settings);
  // 从数据加载人脸图库，提取特征到内存中。
  engine->LoadFeaturesToMem(mysql);
  // 初始化 Rpc 服务器。
  arm_face_id::RpcServer server(absl::GetFlag(FLAGS_server_addr));

  // 注册请求处理的回调方法。
  server.RegisterRPCHandler<RecognitionRequest, RecognitionResponse>(
      [=](RecognitionRequest& req, /* 从客户端接受到的请求消息 */
          RecognitionResponse& resp /* 将要返回的响应消息 */) {
        QByteArray byte_arr(req.image().data(), req.image().size());
        QImage qimage;
        qimage.loadFromData(byte_arr);
        qimage = qimage.convertToFormat(QImage::Format_RGB888);
        cv::Mat mat = arm_face_id::utils::qimage_to_mat(qimage);

        int64_t res = engine->RecognizeFaceFromMem(
            SeetaImageData{mat.cols, mat.rows, mat.channels(), mat.data});

        if (res == -3) {
          return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                              "图像格式错误");
        } else if (res == -1) {
          return grpc::Status(grpc::StatusCode::OK, "没有识别到身份");
        }

        MySqlSchema mysql(db_ip, db_port, db_name, db_username, db_password);
        arm_face_id::domain::User result_user = mysql.GetUserByID(res);
        QByteArray profile_pic_bytes;
        QDataStream profile_pic_stream(&profile_pic_bytes,
                                       QIODevice::WriteOnly);
        profile_pic_stream << result_user.face_img;

        resp.mutable_res()->set_user_id(result_user.user_id);
        resp.mutable_res()->set_user_name(result_user.user_name);
        resp.mutable_res()->set_email(result_user.email);
        resp.mutable_res()->set_profile_picture(
            profile_pic_bytes.toStdString());
        resp.mutable_res()->set_last_recognized_datetime(
            absl::FormatTime(absl::Now()));

        return grpc::Status(grpc::StatusCode::OK, "识别成功");
      });

  // TODO 人脸注册接口
  server.RegisterRPCHandler<RegistrationRequest, RegistrationResponse>(
      [&](RegistrationRequest& req, RegistrationResponse& resp) {
        // qbytearray -> qimage -> mat -> seetaimage
        QByteArray byte_arr(req.info().face_image().data(),
                            req.info().face_image().size());
        QImage face_qimg;
        face_qimg.loadFromData(byte_arr);
        face_qimg = face_qimg.convertToFormat(QImage::Format_RGB888);
        cv::Mat mat = arm_face_id::utils::qimage_to_mat(face_qimg);
        SeetaImageData face_simg{mat.cols, mat.rows, mat.channels(), mat.data};

        std::vector<SeetaFaceInfo> face_infos = engine->DetectFaces(face_simg);
        if (face_infos.empty()) {
          return grpc::Status(grpc::INVALID_ARGUMENT, "未检测到人脸");
        }

        SeetaFaceInfo face = face_infos.front();
        bool is_exist = engine->CheckIfFaceExist(face_simg, face);
        if (is_exist) {
          return grpc::Status(grpc::INVALID_ARGUMENT, "用户已存在");
        }

        auto quality = engine->AnalyzeQuality(face_simg, face);
        if (quality.clarity <= seeta::MEDIUM)
          return grpc::Status(grpc::INVALID_ARGUMENT, "图片模糊");
        if (quality.integrity <= seeta::MEDIUM)
          return grpc::Status(grpc::INVALID_ARGUMENT, "图片中无完整脸部");
        if (quality.pose == seeta::LOW)
          return grpc::Status(grpc::INVALID_ARGUMENT, "图片无正脸姿势");
        if (quality.resolution <= seeta::MEDIUM)
          return grpc::Status(grpc::INVALID_ARGUMENT, "图片分辨率不足");

        arm_face_id::domain::User registered_user;
        MySqlSchema mysql(db_ip, db_port, db_name, db_username, db_password);

        registered_user.user_name = req.info().user_name();
        registered_user.email = req.info().email();
        registered_user.face_img = face_qimg;
        // 从用户昵称中生成用户头像
        registered_user.profile_pic = arm_face_id::utils::generate_hash_avatar(
            QString::fromStdString(registered_user.user_name));
        if (!mysql.SaveUser(registered_user)) {
          return grpc::Status(grpc::INTERNAL, "内部错误");
        }
        // 直接向内存中追加新注册的人脸特征信息，避免高频率地从数据库加载数据
        engine->AddNewFeatureToMem(face_simg, face, registered_user.user_id);

        return grpc::Status(grpc::StatusCode::OK, "用户注册成功");
      });

  std::thread rpc_thread(&arm_face_id::RpcServer::Run, &server);

  // 使用 GUI
  if (absl::GetFlag(FLAGS_gui)) {
    treasure_chest::pattern::SyncQueue<cv::Mat> camera_queue;
    arm_face_id::Camera::Settings cam_settings;
    cam_settings.cam_index = absl::GetFlag(FLAGS_camera_index);
    cam_settings.cam_url = absl::GetFlag(FLAGS_camera_url);
    std::shared_ptr<arm_face_id::Camera> camera(
        new arm_face_id::Camera(cam_settings, camera_queue));

    camera->Open();
    std::thread cam_thread(&arm_face_id::Camera::Start, camera.get());
    cam_thread.detach();

    QApplication app(argc, argv);
    std::shared_ptr<arm_face_id::GUI> gui(new arm_face_id::GUI);
    gui->SetEngine(engine);
    camera->AddObserver<cv::Mat>(gui);
    gui->show();

    rpc_thread.detach();

    return app.exec();
  }

  QCoreApplication app(argc, argv);

  app.exec();
  rpc_thread.join();

  return 0;
}
