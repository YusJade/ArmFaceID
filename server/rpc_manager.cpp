#include "rpc_manager.h"

#include <vector>

grpc::Status arm_face_id::RpcManagerImpl::RecognizeFace(
    grpc::ServerContext* context, const arm_face_id::RecognizeRequest* request,
    arm_face_id::RecognizeResult* response) {
  // 测试内容：返回一张照片
  cv::Mat test_image = cv::imread("./LDH.jpg");
  std::vector<uchar> image_encoded;
  cv::imencode(".jpg", test_image, image_encoded);
  std::string bytes(image_encoded.begin(), image_encoded.end());
  // response->set_face_img(bytes);
  // 直接返回请求里的图片
  response->set_face_img(request->face_img());
  response->set_id("1");
  response->set_name("LDH");
  return grpc::Status::OK;
}
