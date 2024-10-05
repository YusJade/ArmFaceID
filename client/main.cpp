#include <QApplication>
#include <QWidget>
#include <memory>

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>

#include "face_camera.h"
#include "face_processor.h"
#include "gui.h"
#include "interface.h"

#include "face_network.pb.h"

ABSL_FLAG(std::string, model_path, ".",
          "[Required] set the path of cascade classifer model `s path.");
ABSL_FLAG(int, camera_id, -1, "[Optional] set the id of camera device.");
ABSL_FLAG(std::string, network_camera_url, "",
          "[Optional] set the url of network camera.");
ABSL_FLAG(std::string, rpc_server_addr, "",
          "[Required] set the addr of rpc server.");

using arm_face_id::RecognizeResult;
using arm_face_id::interface::FaceDetector;

// "localhost:50051" "http://192.168.11.182:4747/video"
// "E:\\Downloads\\lbpcascade_frontalface_improved.xml"
int main(int argc, char* argv[]) {
  absl::SetProgramUsageMessage(
      "There are the availiable flags for this program:");
  absl::ParseCommandLine(argc, argv);

  QApplication app(argc, argv);

  std::string model_path = absl::GetFlag(FLAGS_model_path);
  std::string network_camera_url = absl::GetFlag(FLAGS_network_camera_url);
  std::string rpc_server_addr = absl::GetFlag(FLAGS_rpc_server_addr);
  int camera_device_id = absl::GetFlag(FLAGS_camera_id);

  std::shared_ptr<arm_face_id::RpcClient> rpc_client =
      std::make_shared<arm_face_id::RpcClient>(grpc::CreateChannel(
          rpc_server_addr, grpc::InsecureChannelCredentials()));
  std::shared_ptr<arm_face_id::FaceProcessor> face_processor_ptr =
      std::make_shared<arm_face_id::FaceProcessor>(rpc_client, model_path);
  face_processor_ptr->Start();
  shared_ptr<arm_face_id::GUI> gui = std::make_shared<arm_face_id::GUI>();

  face_processor_ptr->FaceDetector<RecognizeResult>::AddObserver(gui);
  arm_face_id::FaceCamera::Settings cam_settings;
  cam_settings.cam_index = camera_device_id;
  cam_settings.cam_url = network_camera_url;

  arm_face_id::FaceCamera camera(cam_settings);
  camera.AddObserver(gui);
  camera.AddObserver(face_processor_ptr);
  camera.OpenAndStart();

  gui->show();

  return app.exec();
}
