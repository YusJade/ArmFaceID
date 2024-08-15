#include "display_widget.h"

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

#include "utils.h"

arm_face_id::DisplayWidget::DisplayWidget(const std::string& rpc_server_addr,
                                          FaceProcessor* processor)
    : rpc_client_(std::make_shared<arm_face_id::RpcClient>(grpc::CreateChannel(
          rpc_server_addr, grpc::InsecureChannelCredentials()))),
      processor_(processor) {}

void arm_face_id::DisplayWidget::OnImageCaptured(cv::Mat captureed_image) {
  capture_lbl_->setPixmap(
      QPixmap::fromImage(utils::MatToQImage(captureed_image)));
}

void arm_face_id::DisplayWidget::OnFaceDetected(cv::Mat detected_image,
                                                cv::Rect face_rect) {
  face_lbl_->setPixmap(QPixmap::fromImage(utils::MatToQImage(detected_image)));
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

  QStackedLayout* switch_layout = new QStackedLayout;
  QVBoxLayout* detect_layout = new QVBoxLayout;
  QVBoxLayout* registry_layout = new QVBoxLayout;

  detect_layout->addWidget(name_lbl_);
  detect_layout->addWidget(id_lbl_);

  QLineEdit* edit_name = new QLineEdit;
  QPushButton* register_btn = new QPushButton("reigster");

  QObject::connect(register_btn, QPushButton::clicked, [=] {
    std::thread thread([this, edit_name] {
      ABSL_LOG(INFO) << "preparing register...";
      rpc_client_->Register(utils::QImageToMat(face_lbl_->pixmap().toImage()),
                            edit_name->text().toStdString());
    });
    thread.detach();
    // QtConcurrent::run([this, edit_name] {
    //   ABSL_LOG(INFO) << "preparing register...";
    //   rpc_client_->Register(utils::QImageToMat(face_lbl_->pixmap().toImage()),
    //                         edit_name->text().toStdString());
    // });
  });

  registry_layout->addWidget(edit_name);
  registry_layout->addWidget(register_btn);

  QWidget* detect_widget = new QWidget;
  QWidget* registry_widget = new QWidget;
  detect_widget->setLayout(detect_layout);
  registry_widget->setLayout(registry_layout);

  switch_layout->addWidget(detect_widget);
  switch_layout->addWidget(registry_widget);

  in_vbox_layout_r->addLayout(switch_layout);

  hbox_layout->addLayout(in_vbox_layout_l);
  hbox_layout->addLayout(in_vbox_layout_r);

  QMenuBar* menu_bar = new QMenuBar;
  QMenu* menu = new QMenu("功能");

  QToolButton* to_registry_btn = new QToolButton;
  QToolButton* to_detect_btn = new QToolButton;
  to_registry_btn->setText("registry");
  to_detect_btn->setText("detect");

  QObject::connect(to_registry_btn, QToolButton::clicked,
                   [switch_layout, this] {
                     processor_->Stop();
                     switch_layout->setCurrentIndex(1);
                   });
  QObject::connect(to_detect_btn, QToolButton::clicked, [switch_layout, this] {
    processor_->Continue();
    switch_layout->setCurrentIndex(0);
  });

  QWidgetAction* action_registry = new QWidgetAction(widget_);
  QWidgetAction* action_detect = new QWidgetAction(widget_);

  action_registry->setDefaultWidget(to_registry_btn);
  action_detect->setDefaultWidget(to_detect_btn);

  menu->addAction(action_registry);
  menu->addAction(action_detect);

  menu_bar->addMenu(menu);
  hbox_layout->setMenuBar(menu_bar);

  widget_->setLayout(hbox_layout);

  return widget_;
}
