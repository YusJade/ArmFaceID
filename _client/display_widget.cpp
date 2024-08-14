#include "display_widget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <absl/log/absl_log.h>
#include <absl/strings/str_format.h>

#include "utils.h"

arm_face_id::DisplayWidget::DisplayWidget(const std::string& rpc_server_addr)
    : rpc_client(std::make_shared<arm_face_id::RpcClient>(grpc::CreateChannel(
          rpc_server_addr, grpc::InsecureChannelCredentials()))) {}

void arm_face_id::DisplayWidget::OnImageCaptured(cv::Mat captureed_image) {
  capture_lbl_->setPixmap(
      QPixmap::fromImage(utils::matToQImage(captureed_image)));
}

void arm_face_id::DisplayWidget::OnFaceDetected(cv::Mat detected_image,
                                                cv::Rect face_rect) {
  face_lbl_->setPixmap(QPixmap::fromImage(utils::matToQImage(detected_image)));
}

void arm_face_id::DisplayWidget::OnFaceRecognized(RecognitionResult result) {
  ABSL_LOG(INFO) << absl::StrFormat("regonition result: %d, %s", result.id,
                                    result.name);
}

QWidget* arm_face_id::DisplayWidget::InitWidget() {
  if (widget_) {
    return widget_;
  }
  widget_ = new QWidget();
  QHBoxLayout* hbox_layout = new QHBoxLayout();
  QVBoxLayout* in_vbox_layout_l = new QVBoxLayout();
  QVBoxLayout* in_vbox_layout_r = new QVBoxLayout();

  capture_lbl_ = new QLabel("Face Recognition Area");
  face_lbl_ = new QLabel("Result");
  name_lbl_ = new QLabel("name");
  id_lbl_ = new QLabel("id");

  in_vbox_layout_l->addWidget(capture_lbl_);
  in_vbox_layout_l->addWidget(new QLabel("Arm Face Id Project"));

  in_vbox_layout_r->addWidget(face_lbl_);
  in_vbox_layout_r->addWidget(name_lbl_);
  in_vbox_layout_r->addWidget(id_lbl_);

  hbox_layout->addLayout(in_vbox_layout_l);
  hbox_layout->addLayout(in_vbox_layout_r);

  widget_->setLayout(hbox_layout);

  return widget_;
}