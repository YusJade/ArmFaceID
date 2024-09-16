#include <QApplication>
#include <QWidget>
#include <iostream>
#include <memory>
#include <thread>

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <gflags/gflags.h>

#include "camera.h"
#include "face_processor.h"
#include "gui.h"

ABSL_FLAG(std::string, model_path, ".",
          "[Required] set the path of cascade classifer model `s path.");
ABSL_FLAG(int, camera_id, -1, "[Optional] set the id of camera device.");
ABSL_FLAG(std::string, network_camera_url, "",
          "[Optional] set the url of network camera.");
ABSL_FLAG(std::string, rpc_server_addr, "",
          "[Required] set the addr of rpc server.");

// "localhost:50051" "http://192.168.11.182:4747/video"
// "E:\\Downloads\\lbpcascade_frontalface_improved.xml"
int main(int argc, char* argv[]) {
  absl::SetProgramUsageMessage(
      "There are the availiable flags for this program:");
  absl::ParseCommandLine(argc, argv);
  // gflags::SetUsageMessage("There are the availiable flags for this
  // program:");
  // gflags::ParseCommandLineFlags(&argc, &argv, false);

  QApplication app(argc, argv);

  std::string model_path = absl::GetFlag(FLAGS_model_path);
  std::string network_camera_url = absl::GetFlag(FLAGS_network_camera_url);
  std::string rpc_server_addr = absl::GetFlag(FLAGS_rpc_server_addr);
  int camera_device_id = absl::GetFlag(FLAGS_camera_id);

  std::shared_ptr<arm_face_id::RpcClient> rpc_client =
      std::make_shared<arm_face_id::RpcClient>(grpc::CreateChannel(
          rpc_server_addr, grpc::InsecureChannelCredentials()));
  //   arm_face_id::FaceProcessorSetting setting(
  //       camera_device_id, network_camera_url, model_path, rpc_server_addr);

  std::shared_ptr<arm_face_id::FaceProcessor> face_processor_ptr =
      std::make_shared<arm_face_id::FaceProcessor>(rpc_client, model_path);
  arm_face_id::Camera camera;
  if (camera_device_id == -1) {
    camera.Open(network_camera_url);
  } else {
    camera.Open(camera_device_id);
  }
  std::thread camera_thread([&] { camera.Start(); });
  camera_thread.detach();
  std::shared_ptr<arm_face_id::GUI> widget =
      std::make_shared<arm_face_id::GUI>(rpc_client);
  camera.RegisterListener(widget);
  camera.RegisterListener(face_processor_ptr);
  //   face_processor.SetListener(widget);
  QWidget* w = widget->Get();
  //   face_processor.Start();

  w->show();
  return app.exec();
}
