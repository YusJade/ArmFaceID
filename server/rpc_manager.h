#ifndef RPC_MANAGER_H
#define RPC_MANAGER_H

#include <memory>
#include <mutex>

#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>

#include "engine.h"

#include "face_network.grpc.pb.h"
#include "face_network.pb.h"

namespace arm_face_id {

class RpcManagerImpl final : public RpcManager::Service {
 public:
  RpcManagerImpl(std::shared_ptr<FaceDetectorServer> engine_ptr);
  RpcManagerImpl() = delete;
  ~RpcManagerImpl();

  grpc::Status RecognizeFace(grpc::ServerContext* context,
                             const RecognizeRequest* request,
                             RecognizeResult* response) override;

  grpc::Status Register(grpc::ServerContext* context,
                        const RegisterRequest* request,
                        RegisterResult* response) override;

 private:
  std::shared_ptr<FaceDetectorServer> engine_ptr_;
  std::mutex mutex_;
};

}  // namespace arm_face_id

#endif  // RPC_MANAGER_H
