#include "gui.h"

#include <qboxlayout.h>
#include <qgridlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
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
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "function.h"

using namespace arm_face_id;

GUI::GUI() {
  QWidget* main_widget = new QWidget;
  main_widget->setMinimumSize(1920 * 0.3, 1080 * 0.3);
  QGridLayout* main_layout = new QGridLayout(main_widget);
  QLabel* cap_label = new QLabel;
  cap_label->setMinimumSize(640, 320);
  camera_frame_lbl_ = cap_label;

  QGroupBox* info_box = new QGroupBox;
  info_box->setTitle("识别结果");
  info_box->setMinimumWidth(240);
  QGridLayout* info_layout = new QGridLayout;
  info_box->setLayout(info_layout);

  QLabel* nick_name_label = new QLabel("昵称");
  nick_name_val_label_ = new QLabel;

  QLabel* email_label = new QLabel("邮箱");
  email_val_label_ = new QLabel;

  info_layout->addWidget(nick_name_label, 1, 0);
  info_layout->addWidget(nick_name_val_label_, 1, 1);
  info_layout->addWidget(email_label, 2, 0);
  info_layout->addWidget(email_val_label_, 2, 1);
  info_layout->setRowStretch(0, 4);
  info_layout->setRowStretch(1, 1);
  info_layout->setRowStretch(2, 1);
  info_layout->setColumnStretch(0, 1);
  info_layout->setColumnStretch(1, 3);

  main_layout->addWidget(cap_label, 1, 0);
  main_layout->addWidget(info_box, 1, 1);
  main_layout->setColumnStretch(0, 3);
  main_layout->setColumnStretch(1, 3);

  setCentralWidget(main_widget);
  adjustSize();
  // setMinimumSize(1920 * 0.3, 1080 * 0.3);
}

GUI::GUI(std::shared_ptr<RpcClient> rpc_client_ptr)
    : rpc_client_(rpc_client_ptr) {}

void GUI::OnFrameCaptured(cv::Mat frame) {
  if (camera_frame_lbl_) {
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    auto img = QPixmap::fromImage(utils::mat_to_qimage(frame));
    img = img.scaled(camera_frame_lbl_->size(), Qt::KeepAspectRatio);
    camera_frame_lbl_->setPixmap(img);

    if (img.size() != camera_frame_lbl_->size()) {
      camera_frame_lbl_->setMinimumSize(img.size());
      camera_frame_lbl_->setFixedSize(img.size());
    }
  }
}

void GUI::OnFaceDetected(cv::Mat img, vector<cv::Rect> faces) {}

void GUI::OnFaceRecognized(cv::Mat img, cv::Rect face, int64_t id) {
  if (nick_name_val_label_) {
    // nick_name_val_label_->setText();
  }
}

void GUI::OnFaceRecognized(cv::Mat img, cv::Rect face, RecognizeResult* res) {
  if (nick_name_val_label_) {
    nick_name_val_label_->setText(QString::fromStdString(res->name()));
  }
}

QWidget* GUI::Get() {
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
