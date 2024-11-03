#include "gui.h"

#include <qimage.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qstringview.h>

#include <QMessageBox>
#include <cstdint>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <spdlog/spdlog.h>

#include "face_database.h"
#include "seeta/Common/CStruct.h"
#include "utils/base.h"

using namespace arm_face_id;

GUI::GUI() {
  SetupUi();
  model_register_radio_button->setChecked(true);
  SwitchRegisterModel();
  is_recognize_model = false;
}

void GUI::OnNotify(const cv::Mat &message) {
  cv::Mat frame = message.clone();
  QImage img = utils::mat_to_qimage(frame);
  img = img.scaled(video_widget->size(), Qt::AspectRatioMode::KeepAspectRatio);
  video_widget->setPixmap(QPixmap::fromImage(img));

  if (!engine_) return;
  SeetaImageData simg{frame.cols, frame.rows, frame.channels(), frame.data};
  auto faces = engine_->DetectFaces(simg);
  if (!faces.empty()) {
    auto top_face = faces.front();
    cv::Rect face_rect{top_face.pos.x, top_face.pos.y, top_face.pos.width,
                       top_face.pos.height};
    face_rect.width = face_rect.x + face_rect.width > frame.cols
                          ? frame.cols - face_rect.x - 1
                          : face_rect.width;
    face_rect.height = face_rect.y + face_rect.height > frame.rows
                           ? frame.rows - face_rect.y - 1
                           : face_rect.height;
    cv::Mat face_mat = frame(face_rect);
    QImage face_img = utils::mat_to_qimage(face_mat);
    user_info_label->setPixmap(QPixmap::fromImage(face_img.scaled(
        user_info_label->size(), Qt::AspectRatioMode::KeepAspectRatio)));

    current_face_image_ = simg;
    current_face_info_ = top_face;
  }
}

void GUI::SwitchRegisterModel() {
  submit_button->setEnabled(true);
  submit_button->setText("注册");
  recognition_date_line_edit->setEnabled(false);
  username_date_line_edit->setEnabled(true);
  email_line_edit->setEnabled(true);
}

void GUI::SwitchRecognizeModel() {
  submit_button->setEnabled(false);
  submit_button->setText("注册");
  recognition_date_line_edit->setEnabled(false);
  username_date_line_edit->setEnabled(false);
  email_line_edit->setEnabled(false);
}

void GUI::SetupConnection() {
  connect(model_register_radio_button, &QRadioButton::toggled, this,
          [this](bool is_toggled) {
            if (is_toggled) SwitchRegisterModel();
          });
  connect(model_recognize_radio_button, &QRadioButton::toggled, this,
          [this](bool is_toggled) {
            if (is_toggled) SwitchRecognizeModel();
          });
  connect(submit_button, &QPushButton::clicked, this, [this] {
    if (!engine_) return;
    std::string username = username_date_line_edit->text().toStdString();
    std::string email = email_line_edit->text().toStdString();
    QImage profile_pic =
        utils::generate_hash_avatar(QString::fromStdString(username));
    cv::Mat mat{current_face_image_.height, current_face_image_.width,
                CV_8UC(current_face_image_.channels), current_face_image_.data};
    QImage face_qimg = utils::mat_to_qimage(mat);
    data::User new_user{-1, username, email, profile_pic, face_qimg};
    int64_t id = engine_->RegisterFace(current_face_image_, current_face_info_,
                                       new_user);
    if (id == -1) {
      QMessageBox::information(this, "注册", "注册失败！");
    } else {
      QMessageBox::information(this, "注册", "注册成功！");
    }
  });
}

void GUI::SetupUi() {
  if (objectName().isEmpty()) setObjectName("ArmFaceID-server");
  resize(895, 545);
  QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
  setSizePolicy(sizePolicy);
  central_widget = new QWidget(this);
  central_widget->setObjectName("central_widget");
  sizePolicy.setHeightForWidth(
      central_widget->sizePolicy().hasHeightForWidth());
  central_widget->setSizePolicy(sizePolicy);
  horizontal_layout_widget = new QWidget(central_widget);
  horizontal_layout_widget->setObjectName("horizontal_layout_widget");
  horizontal_layout_widget->setGeometry(QRect(9, 9, 881, 511));
  horizontal_layout = new QHBoxLayout(horizontal_layout_widget);
  horizontal_layout->setObjectName("horizontal_layout");
  horizontal_layout->setContentsMargins(0, 0, 0, 0);
  video_display_group_box = new QGroupBox(horizontal_layout_widget);
  video_display_group_box->setObjectName("video_display_group_box");
  // video_display_layout_widget = new QWidget(video_display_group_box);
  // video_display_layout_widget->setObjectName("video_display_layout_widget");
  // video_display_layout_widget->setGeometry(QRect(10, 19, 641, 481));
  video_display_layout = new QHBoxLayout(video_display_group_box);
  video_display_layout->setObjectName("video_display_layout");
  video_display_layout->setContentsMargins(0, 0, 0, 0);
  video_widget = new QLabel(video_display_group_box);
  video_widget->setObjectName("video_label");
  video_display_layout->addWidget(video_widget);

  horizontal_layout->addWidget(video_display_group_box);

  vertical_layout = new QVBoxLayout();
  vertical_layout->setObjectName("vertical_layout");
  user_info_group_box = new QGroupBox(horizontal_layout_widget);
  user_info_group_box->setObjectName("user_info_group_box");
  user_info_group_box->setAlignment(Qt::AlignLeading | Qt::AlignHCenter |
                                    Qt::AlignVCenter);
  user_info_layout_widget = new QWidget(user_info_group_box);
  user_info_layout_widget->setObjectName("user_info_layout_widget");
  user_info_layout_widget->setGeometry(QRect(10, 20, 201, 141));
  user_info_layout = new QVBoxLayout(user_info_layout_widget);
  user_info_layout->setObjectName("user_info_layout");
  user_info_layout->setContentsMargins(0, 0, 0, 0);
  user_info_label = new QLabel(user_info_layout_widget);
  user_info_label->setObjectName("user_info_label");

  user_info_layout->addWidget(user_info_label);

  vertical_layout->addWidget(user_info_group_box);

  settings_group_box = new QGroupBox(horizontal_layout_widget);
  settings_group_box->setObjectName("settings_group_box");
  settings_layout_widget = new QWidget(settings_group_box);
  settings_layout_widget->setObjectName("settings_layout_widget");
  settings_layout_widget->setGeometry(QRect(10, 20, 201, 141));
  settings_layout = new QVBoxLayout(settings_layout_widget);
  settings_layout->setObjectName("settings_layout");
  settings_layout->setContentsMargins(0, 0, 0, 0);
  settings_label = new QLabel(settings_layout_widget);
  settings_label->setObjectName("settings_label");
  settings_label->setScaledContents(false);
  settings_label->setAlignment(Qt::AlignLeading | Qt::AlignLeft |
                               Qt::AlignVCenter);

  settings_layout->addWidget(settings_label);

  vertical_layout->addWidget(settings_group_box);

  input_grid_layout = new QGridLayout();
  input_grid_layout->setObjectName("input_grid_layout");
  email_line_edit = new QLineEdit(horizontal_layout_widget);
  email_line_edit->setObjectName("name_line_edit");

  input_grid_layout->addWidget(email_line_edit, 3, 1, 1, 2);

  email_label = new QLabel(horizontal_layout_widget);
  email_label->setObjectName("name_label");

  input_grid_layout->addWidget(email_label, 3, 0, 1, 1);

  submit_button = new QPushButton(horizontal_layout_widget);
  submit_button->setObjectName("submit_button");
  submit_button->setEnabled(false);

  input_grid_layout->addWidget(submit_button, 5, 0, 1, 3);

  username_date_line_edit = new QLineEdit(horizontal_layout_widget);
  username_date_line_edit->setObjectName("age_line_edit");

  input_grid_layout->addWidget(username_date_line_edit, 2, 1, 1, 2);

  recognition_date_label = new QLabel(horizontal_layout_widget);
  recognition_date_label->setObjectName("age_label");

  input_grid_layout->addWidget(recognition_date_label, 4, 0, 1, 1);

  model_label = new QLabel(horizontal_layout_widget);
  model_label->setObjectName("gender_label");

  input_grid_layout->addWidget(model_label, 1, 0, 1, 1);

  model_recognize_radio_button = new QRadioButton(horizontal_layout_widget);
  model_recognize_radio_button->setObjectName("male_radio_button");

  input_grid_layout->addWidget(model_recognize_radio_button, 1, 2, 1, 1);

  model_register_radio_button = new QRadioButton(horizontal_layout_widget);
  model_register_radio_button->setObjectName("female_radio_button");
  input_grid_layout->addWidget(model_register_radio_button, 1, 1, 1, 1);

  recognition_date_line_edit = new QLineEdit(horizontal_layout_widget);
  recognition_date_line_edit->setObjectName("address_line_edit");
  input_grid_layout->addWidget(recognition_date_line_edit, 4, 1, 1, 2);

  user_name_label = new QLabel(horizontal_layout_widget);
  user_name_label->setObjectName("address_label");

  input_grid_layout->addWidget(user_name_label, 2, 0, 1, 1);
  input_grid_layout->setColumnStretch(0, 1);

  vertical_layout->addLayout(input_grid_layout);
  vertical_layout->setStretch(0, 1);
  vertical_layout->setStretch(1, 1);
  vertical_layout->setStretch(2, 1);

  horizontal_layout->addLayout(vertical_layout);
  horizontal_layout->setStretch(0, 3);
  horizontal_layout->setStretch(1, 1);

  setCentralWidget(central_widget);
  menu_bar = new QMenuBar(this);
  menu_bar->setObjectName("menu_bar");
  menu_bar->setGeometry(QRect(0, 0, 895, 22));
  setMenuBar(menu_bar);
  status_bar = new QStatusBar(this);
  status_bar->setObjectName("status_bar");
  setStatusBar(status_bar);

  RetranslateUi();
  SetupConnection();
  // QMetaObject::connectSlotsByName(this);
}  // setupUi

void GUI::RetranslateUi() {
  this->setWindowTitle(
      QCoreApplication::translate("MainWindow", "ArmFaceID-server", nullptr));
  video_display_group_box->setTitle(
      QCoreApplication::translate("MainWindow", "摄像头", nullptr));
  user_info_group_box->setTitle(
      QCoreApplication::translate("MainWindow", "用户信息", nullptr));
  user_info_label->setText(
      QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
  settings_group_box->setTitle(
      QCoreApplication::translate("MainWindow", "头像", nullptr));
  settings_label->setText(
      QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
  email_label->setText(
      QCoreApplication::translate("MainWindow", "邮箱", nullptr));
  submit_button->setText(
      QCoreApplication::translate("MainWindow", "注册", nullptr));
  recognition_date_label->setText(
      QCoreApplication::translate("MainWindow", "上次识别日期", nullptr));
  model_label->setText(
      QCoreApplication::translate("MainWindow", "当前模式", nullptr));
  model_recognize_radio_button->setText(
      QCoreApplication::translate("MainWindow", "识别", nullptr));
  model_register_radio_button->setText(
      QCoreApplication::translate("MainWindow", "注册", nullptr));
  user_name_label->setText(
      QCoreApplication::translate("MainWindow", "用户名", nullptr));

}  // retranslateUi}
