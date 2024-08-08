#include <grpcpp/grpcpp.h>
#include <grpcpp/server_builder.h>

#include "rpc_manager.h"

constexpr char kServerAddrInfo[] = "localhost:50051";

int main(int argc, char *argv[]) {
  arm_face_id::RpcManagerImpl rpc_service;

  grpc::ServerBuilder server_builder;
  server_builder.AddListeningPort(kServerAddrInfo,
                                  grpc::InsecureServerCredentials());
  server_builder.RegisterService(&rpc_service);

  std::unique_ptr<grpc::Server> rpc_server(server_builder.BuildAndStart());
  rpc_server->Wait();

  return 0;
}