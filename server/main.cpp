#include <QApplication>
#include <QWidget>
#include <thread>

#include <absl/log/globals.h>
#include <absl/log/initialize.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server_builder.h>

#include "rpc_manager.h"

constexpr char kServerAddrInfo[] = "localhost:50051";

int main(int argc, char* argv[]) {
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