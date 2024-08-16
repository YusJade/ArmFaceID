#include "rpc_manager.h"

#include <QVBoxLayout>
#include <vector>

#include "utils/utils.h"

constexpr float kThrehold = 0.8;

arm_face_id::RpcManagerImpl::RpcManagerImpl() {
  seeta::ModelSetting FD_model_setting("fd_2_00.dat",
                                       seeta::ModelSetting::Device::GPU, 0);
  seeta::ModelSetting PD_model_setting("pd_2_00_pts5.dat",
                                       seeta::ModelSetting::Device::GPU, 0);
  seeta::ModelSetting FR_model_setting("fr_2_10.dat",
                                       seeta::ModelSetting::Device::GPU, 0);
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
  // 获取开始时间点
  auto start = std::chrono::high_resolution_clock::now();

  int64_t id = face_engine_ptr->Query(seeta_image_data, &similarity);

  // 获取结束时间点
  auto end = std::chrono::high_resolution_clock::now();
  // 计算函数执行的时间间隔
  std::chrono::duration<double, std::milli> duration = end - start;
  // 输出执行时间
  ABSL_LOG(INFO) << "Query execution time: " << duration.count() << " ms"
                 << std::endl;

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

  img_lbl->setPixmap(QPixmap::fromImage(utils::MatToQImage(image)));

  SeetaImageData seeta_img;
  seeta_img.channels = image.channels();
  seeta_img.data = image.data;
  seeta_img.height = image.rows;
  seeta_img.width = image.cols;
  float similarity = 0.0;

  int64_t id = face_engine_ptr->Query(seeta_img, &similarity);
  if (similarity >= kThrehold) {
    std::cerr << "Face registeration already exist!" << std::endl;

    return grpc::Status(grpc::StatusCode::ALREADY_EXISTS, "fail",
                        "face registeration already exist!");
  }

  // 获取开始时间点
  auto start = std::chrono::high_resolution_clock::now();

  id = face_engine_ptr->Register(seeta_img);

  // 获取结束时间点
  auto end = std::chrono::high_resolution_clock::now();
  // 计算函数执行的时间间隔
  std::chrono::duration<double, std::milli> duration = end - start;
  // 输出执行时间
  ABSL_LOG(INFO) << "Register execution time: " << duration.count() << " ms"
                 << std::endl;

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

QWidget* arm_face_id::RpcManagerImpl::DisplayWidget() {
  widget_ = new QWidget;
  img_lbl = new QLabel("img_register");
  QVBoxLayout* layout = new QVBoxLayout;
  widget_->setLayout(layout);
  layout->addWidget(img_lbl);
  return widget_;
}
