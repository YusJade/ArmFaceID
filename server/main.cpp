#include <ElaApplication.h>

#include <QApplication>
#include <QWidget>
#include <memory>
#include <thread>

#include <absl/flags/flag.h>
#include <absl/flags/internal/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <absl/log/globals.h>
#include <absl/log/initialize.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server_builder.h>
#include <seeta/CStruct.h>
#include <seeta/Struct.h>
#include <spdlog/spdlog.h>

#include "ela_gui.h"
#include "engine.h"
#include "face_camera.h"
#include "gui.h"
#include "rpc_manager.h"

/**
 ./build/server/server -native_camera_index=1
 -classifier_path=D:/cmake-install-modules/opencv-4.10.0/etc/haarcascades/haarcascade_frontalface_alt2.xml
 *
 */
ABSL_FLAG(std::string, classifier_path,
          "/home/yu/codefield/opencv/data/haarcascades_cuda/"
          "haarcascade_frontalface_alt2.xml",
          "[REQUIRED] Path of classifer model file.");
ABSL_FLAG(int, native_camera_index, -1, "[OPTIONAL] Index of camera device.");
ABSL_FLAG(std::string, network_camera_url, " ",
          "[OPTIONAL] URL of network camera.");
// ABSL_FLAG(int, native_camera_index, -1, "[Optional] set the id of camera
// device.");

constexpr char kServerAddrInfo[] = "localhost:50051";

int main(int argc, char* argv[]) {
  absl::InitializeLog();
  SetStderrThreshold(absl::LogSeverity::kInfo);
  absl::SetProgramUsageMessage(
      "There are the availiable flags for this program:");
  absl::ParseCommandLine(argc, argv);

  seeta::ModelSetting FD_model_setting("fd_2_00.dat",
                                       seeta::ModelSetting::Device::CPU);
  seeta::ModelSetting PD_model_setting("pd_2_00_pts5.dat",
                                       seeta::ModelSetting::Device::CPU);
  seeta::ModelSetting FR_model_setting("fr_2_10.dat",
                                       seeta::ModelSetting::Device::CPU);
  arm_face_id::FaceDetectorServer::Settings detector_settings;
  detector_settings.fd_setting = FD_model_setting;
  detector_settings.fr_setting = FR_model_setting;
  detector_settings.pd_setting = PD_model_setting;
  detector_settings.classifier_path = absl::GetFlag(FLAGS_classifier_path);
  // engine_config.native_camera_index =
  // absl::GetFlag(FLAGS_native_camera_index); engine_config.network_camera_url
  // = absl::GetFlag(FLAGS_network_camera_url);

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
      Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#else
  // 根据实际屏幕缩放比例更改
  qputenv("QT_SCALE_FACTOR", "1.5");
#endif
#endif
  QApplication app(argc, argv);

  // std::shared_ptr<arm_face_id::FaceDetectorServer> face_detector =
  //     std::make_shared<arm_face_id::FaceDetectorServer>(detector_settings);

  std::shared_ptr<arm_face_id::FaceDetectorServer> face_detector =
      arm_face_id::FaceDetectorServer::BuildAndReturn(detector_settings);

  arm_face_id::RpcManagerImpl rpc_service(face_detector);
  grpc::ServerBuilder server_builder;
  server_builder
      .AddListeningPort(kServerAddrInfo, grpc::InsecureServerCredentials())
      .RegisterService(&rpc_service)
      .SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::MAX_POLLERS,
                           20)
      .SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::MIN_POLLERS,
                           5);

  std::unique_ptr rpc_server(server_builder.BuildAndStart());

  arm_face_id::FaceCamera::Settings cam_settings;
  cam_settings.cam_index = absl::GetFlag(FLAGS_native_camera_index);
  cam_settings.cam_url = absl::GetFlag(FLAGS_network_camera_url);

  arm_face_id::FaceCamera face_cam(cam_settings);
  face_cam.AddObserver(face_detector);
#ifdef ELA_GUI
  std::shared_ptr<arm_face_id::ElaGUI> ela_gui =
      std::make_shared<arm_face_id::ElaGUI>();
  face_detector->AddObserver(ela_gui);

  face_cam.AddObserver(ela_gui);
  if (!face_cam.OpenAndStart()) {
    spdlog::warn("无法启动摄像头子进程！:<");
  }

  spdlog::info("正在启动 Ela GUI~ :>");
  eApp->init();
  ela_gui->show();
  ela_gui->adjustSize();

  std::thread rpc_thread([&] {
    spdlog::info("已启动 gRPC 服务器");
    rpc_server->Wait();
  });
  rpc_thread.detach();

  return app.exec();
#endif

  // arm_face_id::RpcManagerImpl rpc_service;

  // std::shared_ptr<arm_face_id::GUI> gui_ptr =
  //     std::make_shared<arm_face_id::GUI>(engine_ptr);
  // gui_ptr->Get()->show();

  // engine_ptr->RegisterICamera(gui_ptr);
  // engine_ptr->RegisterIListener(gui_ptr);
  // engine_ptr->Start();

  return app.exec();
}
