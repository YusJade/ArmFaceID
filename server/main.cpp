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
  absl::InitializeLog();
  absl::SetStderrThreshold(absl::LogSeverity::kInfo);
  arm_face_id::RpcManagerImpl rpc_service;

  grpc::ServerBuilder server_builder;
  server_builder.AddListeningPort(kServerAddrInfo,
                                  grpc::InsecureServerCredentials());
  server_builder.RegisterService(&rpc_service);

  std::unique_ptr<grpc::Server> rpc_server(server_builder.BuildAndStart());

  QApplication app(argc, argv);
  QWidget* widget = rpc_service.DisplayWidget();
  widget->show();

  std::thread thread([&] { rpc_server->Wait(); });
  thread.detach();

  return app.exec();
}