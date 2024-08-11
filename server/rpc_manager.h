#ifndef RPC_MANAGER_H
#define RPC_MANAGER_H

#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>
#include <opencv2/opencv.hpp>

#include "face_engine.h"

#include "face_network.grpc.pb.h"
#include "face_network.pb.h"

namespace arm_face_id {

class RpcManagerImpl final : public arm_face_id::RpcManager::Service {
 public:
  RpcManagerImpl();

  ::grpc::Status RecognizeFace(::grpc::ServerContext* context,
                               const arm_face_id::RecognizeRequest* request,
                               arm_face_id::RecognizeResult* response) override;

  ::grpc::Status Register(::grpc::ServerContext* context,
                          const ::arm_face_id::RegisterRequest* request,
                          ::arm_face_id::RegisterResult* response) override;

 private:
  std::unique_ptr<::seeta::FaceEngine> face_engine_ptr;
};

}  // namespace arm_face_id

#endif  // RPC_MANAGER_H