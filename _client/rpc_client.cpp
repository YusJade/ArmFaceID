#include "rpc_client.h"

#include "utils.h"

arm_face_id::RpcClient::RpcClient(std::shared_ptr<::grpc::Channel> channel)
    : stub(RpcManager::NewStub(channel)) {}

cv::Mat arm_face_id::RpcClient::RecognizeFace(cv::Mat face_img) {
  arm_face_id::RecognizeRequest request;
  arm_face_id::RecognizeResult result;
  grpc::ClientContext context;

  std::vector<uchar> bytes;
  utils::encodeMat(face_img, ".jpg", bytes);
  std::string bytes_str(bytes.begin(), bytes.end());
  request.set_face_img(bytes_str);

  // TODO: 为何即用指针传递又用引用传递？
  grpc::Status status = stub->RecognizeFace(&context, request, &result);

  cv::Mat decoded_img;
  if (status.ok()) {
    std::cerr << "Success to complete a rpc request.\n";
    // TODO: face_img 为空报错？
    utils::decodeMat(result.face_img(), decoded_img);
  } else {
    std::cerr << "Fail to complete a rpc request!\n";
  }
  return decoded_img;
}

int64_t arm_face_id::RpcClient::Register(cv::Mat image, std::string name) {
  arm_face_id::RegisterRequest request;
  arm_face_id::RegisterResult result;
  grpc::ClientContext context;

  std::vector<uchar> bytes;
  utils::encodeMat(image, ".jpg", bytes);

  request.set_face_img(std::string(bytes.begin(), bytes.end()));

  grpc::Status status = stub->Register(&context, request, &result);

  return result.id();
}
