#include "rpc_manager.h"

#include <cstdint>

#include <grpcpp/support/status.h>
#include <spdlog/spdlog.h>

#include "face_database.h"
#include "function.h"

constexpr float kThrehold = 0.8;

arm_face_id::RpcManagerImpl::RpcManagerImpl(
    std::shared_ptr<arm_face_id::FaceDetectorServer> engine_ptr)
    : engine_ptr_(engine_ptr) {}

arm_face_id::RpcManagerImpl::~RpcManagerImpl() {}

grpc::Status arm_face_id::RpcManagerImpl::RecognizeFace(
    grpc::ServerContext* context, const arm_face_id::RecognizeRequest* request,
    RecognizeResult* response) {
  // spdlog::info("Recieve a request");
  const std::string img_byte_seq = request->face_img();
  cv::Mat decoded_mat;
  utils::bytes_to_mat(img_byte_seq, decoded_mat);
  int64_t id = -2;
  data::User user_info;
  user_info = engine_ptr_->RecognizeFaceFromDb(decoded_mat);

  response->set_id(user_info.id);
  response->set_name(user_info.nick_name);
  return grpc::Status::OK;
}

/**
 * @brief
 *
 * @param context
 * @param request
 * @param response
 * @return grpc::Status
 */
grpc::Status arm_face_id::RpcManagerImpl::Register(
    // TODO: 暂时不提供注册服务
    grpc::ServerContext* context, const arm_face_id::RegisterRequest* request,
    arm_face_id::RegisterResult* response) {
  const std::string img_byte_seq = request->face_img();
  cv::Mat decoded_mat;
  utils::bytes_to_mat(img_byte_seq, decoded_mat);
  // int64_t id = engine_ptr_->RegisterFace(decoded_mat);
  // if (id == -1) {
  //   return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "face not found.");
  // }
  return grpc::Status::OK;
}
