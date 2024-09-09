#include <QApplication>
#include <QWidget>
#include <iostream>

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <gflags/gflags.h>

#include "display_widget.h"
#include "face_processor.h"

ABSL_FLAG(std::string, model_path, ".",
          "[Required] set the path of cascade classifer model `s path.");
ABSL_FLAG(int, camera_id, 1, "[Optional] set the id of camera device.");
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

  // std::shared_ptr<arm_face_id::RpcClient> rpc_client =
  //     std::make_shared<arm_face_id::RpcClient>(grpc::CreateChannel(
  //         rpc_server_addr, grpc::InsecureChannelCredentials()));
  arm_face_id::FaceProcessorSetting setting(
      camera_device_id, network_camera_url, model_path, rpc_server_addr);

  arm_face_id::FaceProcessor face_processor(setting);
  std::shared_ptr<arm_face_id::DisplayWidget> widget =
      std::make_shared<arm_face_id::DisplayWidget>(rpc_server_addr,
                                                   &face_processor);

  face_processor.SetListener(widget);
  QWidget* w = widget->InitWidget();
  face_processor.Start();

  w->show();
  return app.exec();
}
