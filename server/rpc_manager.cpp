#include "rpc_manager.h"

#include <QApplication>
#include <QGraphicsScene>
#include <QLineEdit>
#include <QPushButton>
#include <QToolBar>
#include <QVBoxLayout>
#include <thread>
#include <vector>

#include "utils/utils.h"

constexpr float kThrehold = 0.8;

arm_face_id::RpcManagerImpl::RpcManagerImpl() : cam_img_lbl_(new QLabel) {
  seeta::ModelSetting FD_model_setting("fd_2_00.dat",
                                       seeta::ModelSetting::Device::CPU);
  seeta::ModelSetting PD_model_setting("pd_2_00_pts5.dat",
                                       seeta::ModelSetting::Device::CPU);
  seeta::ModelSetting FR_model_setting("fr_2_10.dat",
                                       seeta::ModelSetting::Device::CPU);
  face_classifier_.load(
      "/usr/local/share/opencv4/haarcascades/haarcascade_frontalface_alt2.xml");
  face_engine_ptr = std::make_unique<seeta::FaceEngine>(
      FD_model_setting, PD_model_setting, FR_model_setting);

  std::thread register_thread([this]() {
    cv::VideoCapture capture;
    // capture.open("http://192.168.3.2:4747/video");
    capture.open(1);
    cv::Mat frame;
    while (true) {
      capture >> frame;
      if (!frame.empty()) {
        std::vector<cv::Rect> faces;
        face_classifier_.detectMultiScale(frame, faces);
        if (!faces.empty()) {
          auto face_rect = faces.front();
          cv::rectangle(frame, face_rect, cv::Scalar(0, 255, 255));
          native_register_img_ = frame;
        }
        cam_img_lbl_->setPixmap(QPixmap::fromImage(utils::MatToQImage(frame)));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    }
  });
  register_thread.detach();
}

arm_face_id::RpcManagerImpl::~RpcManagerImpl() { delete rpc_widget_; }

grpc::Status arm_face_id::RpcManagerImpl::RecognizeFace(
    grpc::ServerContext* context, const arm_face_id::RecognizeRequest* request,
    RecognizeResult* response) {
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
  SeetaImageData seeta_image_data{};

  img_lbl_->setPixmap(QPixmap::fromImage(utils::MatToQImage(img_decoded)));

  seeta_image_data.channels = img_decoded.channels();
  seeta_image_data.width = img_decoded.cols;
  seeta_image_data.height = img_decoded.rows;
  seeta_image_data.data = img_decoded.data;

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

  img_lbl_->setPixmap(QPixmap::fromImage(utils::MatToQImage(image)));

  SeetaImageData seeta_img{};
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

QWidget* arm_face_id::RpcManagerImpl::InitRpcWidget() {
  rpc_widget_ = new QWidget;
  img_lbl_ = new QLabel("img_register");
  img_lbl_->setFixedSize(640, 480);
  img_lbl_->setAlignment(Qt::AlignCenter);

  QVBoxLayout* layout = new QVBoxLayout(rpc_widget_);
  layout->addWidget(img_lbl_);

  QPushButton* button = new QPushButton(rpc_widget_);
  layout->addWidget(button);

  rpc_widget_->setLayout(layout);
  return rpc_widget_;
}

QWidget* arm_face_id::RpcManagerImpl::Widget() {
  display_widget_ = new QWidget();
  display_widget_->setObjectName("display_widget");
  stacked_widget_ = new QStackedWidget(display_widget_);
  // 创建工具栏
  QToolBar* tool_bar = new QToolBar("Main Toolbar", display_widget_);

  // 创建工具栏按钮
  QAction* rpc_action = tool_bar->addAction("RPC");
  QAction* register_action = tool_bar->addAction("Register");
  QAction* load_action = tool_bar->addAction("Load");
  QAction* save_action = tool_bar->addAction("Save");

  QObject::

      // 将按钮与槽函数关联
      QObject::connect(rpc_action, &QAction::triggered,
                       [&] { stacked_widget_->setCurrentWidget(rpc_widget_); });
  QObject::connect(register_action, &QAction::triggered, [&] {
    stacked_widget_->setCurrentWidget(register_widget_);
  });

  // 添加widget到stackedWidget
  stacked_widget_->addWidget(rpc_widget_);
  stacked_widget_->addWidget(register_widget_);

  // 创建布局
  QVBoxLayout* layout = new QVBoxLayout(display_widget_);
  layout->addWidget(tool_bar);
  layout->addWidget(stacked_widget_);
  display_widget_->setLayout(layout);
  return display_widget_;
}

QWidget* arm_face_id::RpcManagerImpl::InitRegisterWidget() {
  register_widget_ = new QWidget();
  register_widget_->setObjectName("register_widget");
  QHBoxLayout* layout = new QHBoxLayout(register_widget_);
  QVBoxLayout* cam_lay = new QVBoxLayout();
  QVBoxLayout* op_lay = new QVBoxLayout();
  layout->addLayout(cam_lay);
  layout->addLayout(op_lay);

  QLabel* cam_name_lbl = new QLabel(register_widget_);
  cam_img_lbl_->setAlignment(Qt::AlignCenter);
  cam_img_lbl_->setFixedSize(640, 480);
  cam_name_lbl->setText("image from cam.");
  cam_lay->addWidget(cam_name_lbl);
  cam_lay->addWidget(cam_img_lbl_);

  QLabel* captured_img_lbl = new QLabel(register_widget_);
  QLabel* input_name_label = new QLabel(register_widget_);
  QLineEdit* input_name = new QLineEdit(register_widget_);
  QPushButton* register_button = new QPushButton(register_widget_);
  QLabel* id_lbl = new QLabel(register_widget_);
  input_name_label->setText("your name:");
  op_lay->addWidget(captured_img_lbl);
  op_lay->addWidget(input_name_label);
  op_lay->addWidget(input_name);
  op_lay->addWidget(register_button);
  op_lay->addWidget(id_lbl);

  QObject::connect(register_button, &QPushButton::clicked,
                   [this, id_lbl, captured_img_lbl] {
                     QPixmap img = cam_img_lbl_->pixmap();
                     // captured_img_lbl->setPixmap(img);
                     cv::Mat img_mat = utils::QImageToMat(img.toImage());
                     std::vector<cv::Rect> faces;
                     cv::Mat gray_img;
                     cv::cvtColor(img_mat, gray_img, cv::COLOR_BGR2GRAY);
                     cv::equalizeHist(gray_img, gray_img);
                     face_classifier_.detectMultiScale(gray_img, faces);
                     if (!faces.empty()) {
                       cv::Mat clip = native_register_img_;
                       cv::imshow("convert", clip);
                       SeetaImageData data{};
                       data.channels = clip.channels();
                       data.width = clip.cols;
                       data.height = clip.rows;
                       data.data = clip.data;
                       int64_t id = face_engine_ptr->Register(data);
                       if (id >= 0) {
                         id_lbl->setText(QString::number(id));
                       }
                     }
                   });

  register_widget_->setLayout(layout);
  return register_widget_;
}
