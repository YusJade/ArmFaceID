#include "rpc_manager.h"

#include <cstdint>

#include <grpcpp/support/status.h>

#include "utils/utils.h"

constexpr float kThrehold = 0.8;

arm_face_id::RpcManagerImpl::RpcManagerImpl(
    std::shared_ptr<arm_face_id::Engine> engine_ptr)
    : engine_ptr_(engine_ptr) {}

arm_face_id::RpcManagerImpl::~RpcManagerImpl() {}

grpc::Status arm_face_id::RpcManagerImpl::RecognizeFace(
    grpc::ServerContext* context, const arm_face_id::RecognizeRequest* request,
    RecognizeResult* response) {
  const std::string img_byte_seq = request->face_img();
  cv::Mat decoded_mat;
  utils::DecodeMat(img_byte_seq, decoded_mat);
  int64_t id = engine_ptr_->RecognizeFace(decoded_mat);

  response->set_id(id);
  return grpc::Status::OK;
}

grpc::Status arm_face_id::RpcManagerImpl::Register(
    grpc::ServerContext* context, const arm_face_id::RegisterRequest* request,
    arm_face_id::RegisterResult* response) {
  const std::string img_byte_seq = request->face_img();
  cv::Mat decoded_mat;
  utils::DecodeMat(img_byte_seq, decoded_mat);
  int64_t id = engine_ptr_->RegisterFace(decoded_mat);
  if (id == -1) {
    return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "face not found.");
  }
  return grpc::Status::OK;
}
