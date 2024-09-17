#include "gui.h"

#include <qboxlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qwidget.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QStackedLayout>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QtConcurrent>

#include <absl/log/absl_log.h>
#include <absl/strings/str_format.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>

#include "utils.h"

arm_face_id::GUI::GUI(std::shared_ptr<RpcClient> rpc_client_ptr)
    : rpc_client_(rpc_client_ptr) {}

arm_face_id::GUI::GUI(const std::string& rpc_server_addr,
                      FaceProcessor* processor)
    : rpc_client_(std::make_shared<arm_face_id::RpcClient>(grpc::CreateChannel(
          rpc_server_addr, grpc::InsecureChannelCredentials()))),
      processor_(processor) {}

void arm_face_id::GUI::OnFrameCaptured(cv::Mat frame) {
  if (camera_frame_lbl_) {
    camera_frame_lbl_->setPixmap(QPixmap::fromImage(utils::MatToQImage(frame)));
  }
}
void arm_face_id::GUI::OnImageCaptured(cv::Mat captureed_image) {
  // capture_lbl_->setPixmap(
  //     QPixmap::fromImage(utils::MatToQImage(captureed_image)));
}

void arm_face_id::GUI::OnFaceDetected(cv::Mat detected_image,
                                      cv::Rect face_rect) {
  // face_img_ = detected_image;
  cv::rectangle(detected_image, face_rect, cv::Scalar(255, 0, 255));
  camera_frame_lbl_->setPixmap(
      QPixmap::fromImage(utils::MatToQImage(detected_image)));
}

void arm_face_id::GUI::OnFaceRecognized(RecognitionResult result) {
  // ABSL_LOG(INFO) << absl::StrFormat("regonition result: %d, %s", result.id,
  //                                   result.name);
}

QWidget* arm_face_id::GUI::Get() {
  if (main_widget_) return main_widget_;
  main_widget_ = new QWidget;
  QHBoxLayout* main_layout = new QHBoxLayout(main_widget_);
  main_widget_->setLayout(main_layout);

  QGroupBox* img_groupbox = new QGroupBox(main_widget_);
  img_groupbox->setTitle("camera");
  QGroupBox* op_groupbox = new QGroupBox(main_widget_);
  op_groupbox->setTitle("operations");
  main_layout->addWidget(img_groupbox);
  main_layout->addWidget(op_groupbox);

  QVBoxLayout* img_layout = new QVBoxLayout(main_widget_);
  QVBoxLayout* op_layout = new QVBoxLayout(main_widget_);
  img_groupbox->setLayout(img_layout);
  op_groupbox->setLayout(op_layout);

  QLabel* camera_label = new QLabel(main_widget_);
  camera_frame_lbl_ = camera_label;
  img_layout->addWidget(camera_label);
  return main_widget_;
}
