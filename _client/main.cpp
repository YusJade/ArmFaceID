#include <QApplication>
#include <QWidget>
#include <iostream>

#include <gflags/gflags.h>

#include "display_widget.h"
#include "face_processor.h"

DEFINE_string(model_path, ".",
              "[Required] set the path of cascade classifer model `s path.");
DEFINE_int32(camera_id, 0, "[Optional] set the id of camera device.");
DEFINE_string(network_camera_url, "",
              "[Optional] set the url of network camera.");
DEFINE_string(rpc_server_addr, "", "[Required] set the addr of rpc server.");

// "localhost:50051" "http://192.168.11.182:4747/video"
// "E:\\Downloads\\lbpcascade_frontalface_improved.xml"
int main(int argc, char* argv[]) {
  gflags::SetUsageMessage("There are the availiable flags for this program:");
  gflags::ParseCommandLineFlags(&argc, &argv, false);

  QApplication app(argc, argv);

  std::shared_ptr<arm_face_id::RpcClient> rpc_client =
      std::make_shared<arm_face_id::RpcClient>(grpc::CreateChannel(
          FLAGS_rpc_server_addr, grpc::InsecureChannelCredentials()));
  arm_face_id::FaceProcessorSetting setting(
      0, FLAGS_network_camera_url, FLAGS_model_path, FLAGS_rpc_server_addr);

  arm_face_id::FaceProcessor face_processor(setting);
  std::shared_ptr<arm_face_id::DisplayWidget> widget =
      std::make_shared<arm_face_id::DisplayWidget>(FLAGS_rpc_server_addr);

  face_processor.RegisterListener(widget);
  QWidget* w = widget->InitWidget();
  face_processor.Start();

  w->show();
  return app.exec();
}