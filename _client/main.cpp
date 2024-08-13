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

int main(int argc, char* argv[]) {
  gflags::SetUsageMessage("There are the availiable flags for this program:");
  gflags::ParseCommandLineFlags(&argc, &argv, false);

  QApplication app(argc, argv);
  arm_face_id::DisplayWidget widget;

  QWidget* w = widget.InitWidget();
  w->show();
  return app.exec();
}