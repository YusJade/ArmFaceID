#include "rpc_manager.h"

#include <vector>

#include "utils/utils.h"

constexpr float kThrehold = 0.8;

arm_face_id::RpcManagerImpl::RpcManagerImpl() {
  seeta::ModelSetting FD_model_setting("fd_2_00.dat",
                                       seeta::ModelSetting::Device::CPU, 0);
  seeta::ModelSetting PD_model_setting("pd_2_00_pts5.dat",
                                       seeta::ModelSetting::Device::CPU, 0);
  seeta::ModelSetting FR_model_setting("fr_2_10.dat",
                                       seeta::ModelSetting::Device::CPU, 0);
  face_engine_ptr = std::make_unique<seeta::FaceEngine>(
      FD_model_setting, PD_model_setting, FR_model_setting);
}

grpc::Status arm_face_id::RpcManagerImpl::RecognizeFace(
    grpc::ServerContext* context, const arm_face_id::RecognizeRequest* request,
    arm_face_id::RecognizeResult* response) {
  // 互斥锁，线程安全
  std::lock_guard<std::mutex> lock(mutex_);
  // 测试内容：返回一张照片
  cv::Mat test_image = cv::imread("./LDH.jpg");
  // std::vector<uchar> image_encoded;
  // cv::imencode(".jpg", test_image, image_encoded);
  // std::string bytes(image_encoded.begin(), image_encoded.end());
  // response->set_face_img(bytes);
  // // 直接返回请求里的图片
  // response->set_face_img(request->face_img());
  // response->set_id("1");
  // response->set_name("LDH");
  cv::Mat img_decoded;
  utils::DecodeMat(request->face_img(), img_decoded);
  SeetaImageData seeta_image_data;

  seeta_image_data.channels = test_image.channels();
  seeta_image_data.width = test_image.cols;
  seeta_image_data.height = test_image.rows;
  seeta_image_data.data = test_image.data;
  // seeta_image_data.channels = img_decoded.channels();
  // seeta_image_data.width = img_decoded.cols;
  // seeta_image_data.height = img_decoded.rows;
  // seeta_image_data.data = img_decoded.data;

  float similarity = 0.0;
  int64_t id = face_engine_ptr->Query(seeta_image_data, &similarity);
  std::vector<uchar> img_bytes;
  response->set_id(id);
  response->set_face_img(request->face_img());
  return grpc::Status::OK;
}

grpc::Status arm_face_id::RpcManagerImpl::Register(
    grpc::ServerContext* context, const arm_face_id::RegisterRequest* request,
    arm_face_id::RegisterResult* response) {
  // 互斥锁，线程安全
  std::lock_guard<std::mutex> lock(mutex_);
  cv::Mat image;
  utils::DecodeMat(request->face_img(), image);

  SeetaImageData seeta_img{image.cols, image.rows, image.channels(),
                           image.data};
  float similarity = 0.0;

  int64_t id = face_engine_ptr->Query(seeta_img, &similarity);
  if (similarity >= kThrehold) {
    std::cerr << "Face registeration already exist!" << std::endl;

    return grpc::Status(grpc::StatusCode::ALREADY_EXISTS, "fail",
                        "face registeration already exist!");
  }

  id = face_engine_ptr->Register(
      SeetaImageData{image.cols, image.rows, image.channels(), image.data});

  if (id != -1) {
    std::cerr << "Face register id:" << id << std::endl;
    std::vector<uchar> img_bytes;
    utils::EncodeMat(image, ".jpg", img_bytes);
    response->set_face_img(std::string(img_bytes.begin(), img_bytes.end()));
    response->set_id(id);
    response->set_name(request->name());

    return grpc::Status::OK;
  }

  std::cerr << "Face register fail!" << std::endl;

  return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "fail",
                      "fail to register face!");
}
