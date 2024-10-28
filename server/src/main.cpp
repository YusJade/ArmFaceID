
#include <QApplication>
#include <memory>
#include <thread>

#include <absl/flags/flag.h>
#include <absl/flags/internal/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <absl/log/globals.h>
#include <absl/log/initialize.h>
#include <opencv2/core/mat.hpp>
#include <spdlog/spdlog.h>

#include "camera.h"
#include "engine.h"
#include "face_database.h"
#include "grpc/include/rpc_server.h"
#include "gui.h"
#include "sync_queue/core.h"

ABSL_FLAG(int, camera_index, 0, "本地摄像头 index");
ABSL_FLAG(std::string, camera_url, " ", "网络摄像头 url");
ABSL_FLAG(std::string, server_addr, "127.0.0.1:25575", "RPC 服务器 url");

int main(int argc, char* argv[]) {
  absl::SetProgramUsageMessage("可用参数：");
  absl::ParseCommandLine(argc, argv);

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

  treasure_chest::pattern::SyncQueue<cv::Mat> camera_queue;
  arm_face_id::Camera::Settings cam_settings;
  cam_settings.cam_index = absl::GetFlag(FLAGS_camera_index);
  cam_settings.cam_url = absl::GetFlag(FLAGS_camera_url);
  std::shared_ptr<arm_face_id::Camera> camera(
      new arm_face_id::Camera(cam_settings, camera_queue));

  arm_face_id::data::DBConnection::InitializeDatabase();

  // 初始化 Rpc 服务器
  arm_face_id::RpcServer server(absl::GetFlag(FLAGS_server_addr));
  std::thread rpc_thread(&arm_face_id::RpcServer::Run, &server);
  rpc_thread.detach();

  auto engine = std::make_shared<arm_face_id::FaceEngine>(engine_settings);

  QApplication app(argc, argv);
  std::shared_ptr<arm_face_id::GUI> gui(new arm_face_id::GUI);
  gui->SetEngine(engine);
  camera->AddObserver<cv::Mat>(gui);
  gui->show();

  camera->Open();
  std::thread cam_thread(&arm_face_id::Camera::Start, camera.get());
  cam_thread.detach();

  return app.exec();
}
