#ifndef RPC_CLIENT_H
#define RPC_CLIENT_H

#include <grpcpp/grpcpp.h>
#include <opencv2/opencv.hpp>

#include "face_network.grpc.pb.h"
#include "face_network.pb.h"

namespace arm_face_id {

using arm_face_id::RpcManager;

class RpcClient {
 public:
  RpcClient(std::shared_ptr<::grpc::Channel> channel);

  RecognizeResult RecognizeFace(cv::Mat);
  int64_t Register(cv::Mat, std::string);

 private:
  std::unique_ptr<RpcManager::Stub> stub;
};

}  // namespace arm_face_id
#endif  // RPC_CLIENT_H
