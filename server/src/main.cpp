
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
#include <thread>

#include <absl/flags/flag.h>
#include <absl/flags/internal/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <absl/log/globals.h>
#include <absl/log/initialize.h>
#include <grpcpp/support/status.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include "camera.h"
#include "engine.h"
#include "face_database.h"
#include "grpc/include/rpc_server.h"
#include "gui.h"
#include "seeta/CFaceInfo.h"
#include "seeta/Common/CStruct.h"
#include "sync_queue/core.h"
#include "utils/base.h"

#include "face.pb.h"

using arm_face_id::RecognitionRequest;
using arm_face_id::RecognitionResponse;
using arm_face_id::RegistrationRequest;
using arm_face_id::RegistrationResponse;
using arm_face_id::data::User;

ABSL_FLAG(int, camera_index, 0, "本地摄像头 index");
ABSL_FLAG(std::string, camera_url, " ", "网络摄像头 url");
ABSL_FLAG(std::string, server_addr, "127.0.0.1:25575", "RPC 服务器 url");
ABSL_FLAG(bool, gui, false, "是否使用 GUI");

int main(int argc, char* argv[]) {
  absl::SetProgramUsageMessage("可用参数：");
  absl::ParseCommandLine(argc, argv);
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] [%s:%#] %v");
  SPDLOG_DEBUG("日志器初始化");

  seeta::ModelSetting FD_model_setting("sf3.0_models/face_detector.csta",
                                       SeetaDevice::SEETA_DEVICE_GPU);
  seeta::ModelSetting FR_model_setting("sf3.0_models/face_recognizer.csta",
                                       SeetaDevice::SEETA_DEVICE_GPU);
  seeta::ModelSetting PD_model_setting(
      "sf3.0_models/face_landmarker_pts68.csta", SeetaDevice::SEETA_DEVICE_GPU);
  arm_face_id::FaceEngine::Settings engine_settings;
  engine_settings.fd_setting = FD_model_setting;
  engine_settings.fr_setting = FR_model_setting;
  engine_settings.pd_setting = PD_model_setting;

  arm_face_id::data::DBConnection::InitializeDatabase();
  auto engine = std::make_shared<arm_face_id::FaceEngine>(engine_settings);
  // 从数据加载人脸图库，提取特征到内存中。
  engine->InitializeFeatures();
  // 初始化 Rpc 服务器。
  arm_face_id::RpcServer server(absl::GetFlag(FLAGS_server_addr));

  // 注册请求处理的回调方法。
  server.RegisterRPCHandler<RecognitionRequest, RecognitionResponse>(
      [=](RecognitionRequest& req, /* 从客户端接受到的请求消息 */
          RecognitionResponse& resp /* 将要返回的响应消息 */) {
        auto resp_begin_time_point = std::chrono::high_resolution_clock::now();
        SPDLOG_INFO("接受到一个识别请求");
        QByteArray byte_arr(req.image().data(), req.image().size());
        // byte_arr = QByteArray::fromBase64(byte_arr);
        QImage qimage;
        qimage.loadFromData(byte_arr);
        qimage = qimage.convertToFormat(QImage::Format_RGB888);
        cv::Mat mat = arm_face_id::utils::qimage_to_mat(qimage);

        User res = engine->RecognizeFaceFromDb(
            SeetaImageData{mat.cols, mat.rows, mat.channels(), mat.data});

        if (res.id == -3) {
          return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                              "图像格式错误");
        } else if (res.id == -1) {
          return grpc::Status(grpc::StatusCode::OK, "没有识别到身份");
        }

        resp.mutable_res()->set_user_id(res.id);
        resp.mutable_res()->set_user_name(res.user_name);
        resp.mutable_res()->set_email(res.email);
        resp.mutable_res()->set_face_image(resp.res().face_image());
        resp.mutable_res()->set_profile_picture(resp.res().face_image());
        resp.mutable_res()->set_last_recognized_datetime(
            resp.res().face_image());

        auto resp_finish_time_point = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> took_time =
            resp_finish_time_point - resp_begin_time_point;

        SPDLOG_INFO("处理一条人脸识别请求，耗时 {} s", took_time.count());

        return grpc::Status(grpc::StatusCode::OK, "识别成功");
      });

  // TODO 人脸注册接口
  server.RegisterRPCHandler<RegistrationRequest, RegistrationResponse>(
      [&](RegistrationRequest& req, RegistrationResponse& resp) {
        auto resp_begin_time_point = std::chrono::high_resolution_clock::now();
        SPDLOG_INFO("接受到一个注册请求");
        QByteArray byte_arr(req.info().face_image().data(),
                            req.info().face_image().size());
        // byte_arr = QByteArray::fromBase64(byte_arr);
        QImage qimage;
        qimage.loadFromData(byte_arr);
        qimage = qimage.convertToFormat(QImage::Format_RGB888);
        cv::Mat mat = arm_face_id::utils::qimage_to_mat(qimage);
        SeetaImageData image_data{mat.cols, mat.rows, mat.channels(), mat.data};
        // 检测人脸
        std::vector<SeetaFaceInfo> face_infos = engine->DetectFaces(image_data);
        if (face_infos.empty()) {
          return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                              "未检测到人脸");
        }
        // 使用检测到的人脸信息
        SeetaFaceInfo face_info = face_infos.front();

        // 创建用户对象
        User user;
        user.id = req.info().user_id();
        user.user_name = req.info().user_name();
        user.email = req.info().email();
        // user.face_img = QImage::fromData(QByteArray::fromBase64(
        //     req.info().face_image().data(), req.info().face_image().size()));

        int64_t id = engine->RegisterFace(image_data, face_info, user);
        
        if (id == -3) {
          return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                              "图像格式错误");
        }
        if (id == -2) {
          return grpc::Status(grpc::StatusCode::ALREADY_EXISTS,
                              "人脸信息已存在");
        }
        if (id == -1) {
          return grpc::Status(grpc::StatusCode::INTERNAL, "数据库插入失败");
        }


        auto resp_finish_time_point = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> took_time =
            resp_finish_time_point - resp_begin_time_point;
        SPDLOG_INFO("处理一条注册人脸请求，耗时 {} s", took_time.count());

        return grpc::Status(grpc::StatusCode::INTERNAL, "用户注册成功");
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

  rpc_thread.join();

  return 0;
}
