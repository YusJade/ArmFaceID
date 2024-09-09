#include <QApplication>
#include <QWidget>
#include <thread>

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <absl/log/globals.h>
#include <absl/log/initialize.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server_builder.h>

#include "rpc_manager.h"


ABSL_FLAG(std::string, model_path, ".",
          "[Required] set the path of cascade classifer model `s path.");
ABSL_FLAG(int, camera_id, 1, "[Optional] set the id of camera device.");


constexpr char kServerAddrInfo[] = "localhost:50051";

int main(int argc, char* argv[]) {

  absl::SetProgramUsageMessage(
      "There are the availiable flags for this program:");
  absl::ParseCommandLine(argc, argv);

  std::string model_path = absl::GetFlag(FLAGS_model_path);
  int camera_device_id = absl::GetFlag(FLAGS_camera_id);

  QApplication app(argc, argv);

  absl::InitializeLog();
  SetStderrThreshold(absl::LogSeverity::kInfo);

  arm_face_id::RpcManagerImpl rpc_service;

  grpc::ServerBuilder server_builder;
  server_builder.AddListeningPort(kServerAddrInfo,
                                  grpc::InsecureServerCredentials());
  server_builder.RegisterService(&rpc_service);
  const std::unique_ptr rpc_server(server_builder.BuildAndStart());

  rpc_service.InitRegisterWidget();
  rpc_service.InitRpcWidget();
  QWidget* widget = rpc_service.Widget();
  widget->show();

  std::thread thread([&] { rpc_server->Wait(); });
  thread.detach();

  return QApplication::exec();
}
