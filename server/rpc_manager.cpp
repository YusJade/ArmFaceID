#include "rpc_manager.h"

#include <vector>

#include "utils/utils.h"

arm_face_id::RpcManagerImpl::RpcManagerImpl() {
  seeta::ModelSetting FD_model_setting("FD_model_path",
                                       seeta::ModelSetting::Device::CPU);
  seeta::ModelSetting PD_model_setting("PD_model_path",
                                       seeta::ModelSetting::Device::CPU);
  seeta::ModelSetting FR_model_setting("FD_model_path",
                                       seeta::ModelSetting::Device::CPU);
}

grpc::Status arm_face_id::RpcManagerImpl::RecognizeFace(
    grpc::ServerContext* context, const arm_face_id::RecognizeRequest* request,
    arm_face_id::RecognizeResult* response) {
  // 测试内容：返回一张照片
  // cv::Mat test_image = cv::imread("./LDH.jpg");
  // std::vector<uchar> image_encoded;
  // cv::imencode(".jpg", test_image, image_encoded);
  // std::string bytes(image_encoded.begin(), image_encoded.end());
  // response->set_face_img(bytes);
  // // 直接返回请求里的图片
  // response->set_face_img(request->face_img());
  // response->set_id("1");
  // response->set_name("LDH");
  cv::Mat img_decoded;
  utils::decodeMat(request->face_img(), img_decoded);
  SeetaImageData seeta_image_data;
  seeta_image_data.channels = img_decoded.channels();
  seeta_image_data.width = img_decoded.cols;
  seeta_image_data.height = img_decoded.rows;
  seeta_image_data.data = img_decoded.data;

  float similarity = 0.0;
  int64_t faces = face_engine_ptr->Query(seeta_image_data, &similarity);
  return grpc::Status::OK;
}
