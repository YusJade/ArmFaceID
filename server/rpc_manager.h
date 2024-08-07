#ifndef RPC_MANAGER_H
#define RPC_MANAGER_H

#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>
#include <opencv2/opencv.hpp>

#include "face_network.grpc.pb.h"
#include "face_network.pb.h"

namespace arm_face_id {

class RpcManagerImpl final : public arm_face_id::RpcManager::Service {
 public:
  ::grpc::Status RecognizeFace(::grpc::ServerContext* context,
                               const arm_face_id::RecognizeRequest* request,
                               arm_face_id::RecognizeResult* response);
};

}  // namespace arm_face_id

#endif  // RPC_MANAGER_H