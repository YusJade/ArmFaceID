#include "register_page.h"

#include <ElaIconButton.h>
#include <ElaImageCard.h>
#include <ElaLineEdit.h>
#include <ElaProgressBar.h>
#include <ElaScrollPageArea.h>
#include <ElaText.h>
#include <qboxlayout.h>
#include <qfiledialog.h>
#include <qgridlayout.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlayoutitem.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qsize.h>
#include <qtransform.h>
#include <qwidget.h>

#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <spdlog/spdlog.h>

#include "Def.h"
#include "ElaMessageBar.h"
#include "ElaPushButton.h"
#include "ElaScrollPage.h"
#include "engine.h"
#include "face_database.h"

using namespace arm_face_id;

RegisterPage::RegisterPage() {
  // 在构造函数下调用
  // setWindowFlags(Qt::FramelessWindowHint);
  InitPage();
  // resize(300, 125);
  // adjustSize();
}

void RegisterPage::InitPage() {
  // setPageTitleSpacing(1);
  // setTitleVisible(false);
  // ElaPromotionCard *image_card = new ElaPromotionCard();

  // setContentsMargins(20, 20, 20, 20);
  ElaScrollPageArea *main_area = new ElaScrollPageArea(this);
  main_area->setContentsMargins(20, 20, 20, 20);
  main_area->setFixedSize(800, 475);
  // spdlog::warn("register_page: {}*{}", width(), height());
  // QWidget *central_widget = new QWidget;
  // central_widget->setFixedSize(500, 300);
  // central_widget->setContentsMargins(0, 0, 0, 0);
  // central_widget->setFixedSize(100, 100);

  // setCustomWidget(central_widget);

  QGridLayout *main_layout = new QGridLayout(main_area);
  main_layout->setContentsMargins(0, 0, 0, 0);
  // main_layout->setSpacing(10);
  main_layout->setAlignment(Qt::AlignCenter);

  // 摄像头捕捉区域
  QWidget *camera_widget = new QWidget(this);
  QVBoxLayout *cameraLayout = new QVBoxLayout(camera_widget);
  cameraLayout->setContentsMargins(0, 0, 0, 0);
  camera_widget->setContentsMargins(0, 0, 0, 0);

  QLabel *camera_label = new QLabel(camera_widget);
  camera_label->setMinimumSize(400, 250);
  QPixmap cam_pixmap(camera_label->size());
  cam_pixmap.fill(Qt::black);
  camera_label->setPixmap(cam_pixmap);
  cam_frame_lbl_ = camera_label;

  // 用户注册表单
  // QWidget *form_widget = new QWidget(this);
  QGridLayout *form_layout = new QGridLayout();

  ElaText *form_title = new ElaText("注册信息");
  form_title->setTextPixelSize(20);
  form_title->setFixedHeight(20);

  ElaText *icon_text = new ElaText("头像");
  icon_text->setTextPixelSize(14);
  icon_text->setMinimumWidth(50);
  icon_text->setMaximumWidth(50);
  ElaIconButton *icon_btn =
      new ElaIconButton(QPixmap("./server/assets/test2.png"));
  icon_btn->setFixedSize(125, 125);
  // cap_frame_card_ = image_card;
  // cap_frame_card_->setFixedSize(250, 250);
  ElaLineEdit *name_input = new ElaLineEdit;
  ElaText *name_text = new ElaText("昵称");
  name_input->setPlaceholderText("输入你的昵称");
  name_input->setFixedHeight(30);
  name_input->setMinimumSize(120, 30);
  name_input->setMaximumWidth(220);
  name_text->setTextPixelSize(14);
  name_text->setMinimumWidth(50);
  // name_input->setMaximumSize(120, 30);
  ElaLineEdit *email_input = new ElaLineEdit;
  ElaText *email_text = new ElaText("邮箱");
  email_input->setPlaceholderText("输入你的邮箱");
  email_input->setFixedHeight(30);
  email_input->setMaximumWidth(220);
  email_text->setTextPixelSize(14);
  email_text->setMinimumWidth(50);
  // name_input->setFixedSize(120, 30);
  ElaPushButton *register_button = new ElaPushButton("注册");
  register_button->setObjectName("register_btn");

  ElaProgressBar *progress_bar = new ElaProgressBar(this);
  progress_bar->setMinimumWidth(320);
  progress_bar->setMinimum(0);
  progress_bar->setMaximum(100);

  form_layout->addWidget(form_title, 0, 0);
  form_layout->addWidget(icon_text, 1, 0);
  form_layout->addWidget(icon_btn, 1, 1);
  form_layout->addWidget(name_text, 2, 0);
  form_layout->addWidget(name_input, 2, 1);
  form_layout->addWidget(email_text, 3, 0);
  form_layout->addWidget(email_input, 3, 1);
  form_layout->addWidget(register_button, 4, 0, 1, 2);
  form_layout->setContentsMargins(0, 0, 0, 0);

  form_layout->setColumnStretch(0, 1);
  form_layout->setColumnStretch(1, 5);
  form_layout->setColumnStretch(2, 5);

  form_layout->setRowStretch(0, 1);
  form_layout->setRowStretch(1, 1);
  form_layout->setRowStretch(2, 3);
  form_layout->setRowStretch(3, 1);
  form_layout->setVerticalSpacing(20);

  main_layout->setContentsMargins(0, 0, 0, 0);
  main_layout->addWidget(cam_frame_lbl_, 0, 1);
  main_layout->addLayout(form_layout, 0, 0);
  main_layout->addWidget(progress_bar, 1, 0, 1, 4);

  main_layout->setRowStretch(0, 1);
  main_layout->setRowStretch(1, 3);
  main_layout->setRowStretch(2, 3);
  main_layout->setRowStretch(3, 1);
  main_layout->setColumnStretch(0, 1);
  main_layout->setColumnStretch(1, 3);

  // central_widget->setMaximumHeight(200);
  // 设置窗口属性
  // this->setCustomWidget(central_widget);
  // setMaximumHeight(300);

  connect(register_button, &ElaPushButton::clicked,
          [this, name_input, email_input, register_button] {
            if (name_input->text().isEmpty()) {
              ElaMessageBar::information(
                  ElaMessageBarType::PositionPolicy::TopRight, "注册",
                  "昵称不能为空", 3000);
              return;
            }
            if (email_input->text().isEmpty()) {
              ElaMessageBar::information(
                  ElaMessageBarType::PositionPolicy::TopRight, "注册",
                  "邮箱不能为空", 3000);
              return;
            }
            data::User user;
            user.email = email_input->text().toStdString();
            user.nick_name = name_input->text().toStdString();
            user.face_img = cam_frame_lbl_->pixmap().toImage();
            FaceDetectorServer::GetInstance()->NeedRegisterFace(user);

            ElaMessageBar::information(
                ElaMessageBarType::PositionPolicy::TopRight, "注册",
                "注册中，稍后~", 3000);

            register_button->setDisabled(true);
            emit into_register_state();
          });

  connect(icon_btn, &ElaIconButton::clicked, this, [this, icon_btn] {
    auto file_path = QFileDialog::getOpenFileName();
    QImage img;
    if (!img.load(file_path)) {
      return;
    }
    icon_btn->setPixmap(QPixmap::fromImage(img));
  });
}

void RegisterPage::setCaptureFrame(const QPixmap &img) {
  if (cap_frame_card_) {
    cap_frame_card_->setCardPixmap(img);
  }
}

void RegisterPage::setCameraFrame(const QImage &img) {
  if (cam_frame_lbl_) {
    QPixmap ra_pixmap(img.size());
    ra_pixmap.fill(Qt::transparent);
    QPainterPath path;
    path.addRoundedRect(img.rect(), 20, 20);
    QPainter painter(&ra_pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setClipPath(path);
    painter.drawPixmap(img.rect(), QPixmap::fromImage(img));
    painter.drawPixmap(img.rect(), QPixmap::fromImage(img));

    auto pixmap = ra_pixmap.scaled(cam_frame_lbl_->size(), Qt::KeepAspectRatio);
    cam_frame_lbl_->setPixmap(pixmap);

    // 调整 label 的大小
    // if (cam_frame_lbl_->size() != pixmap.size()) {
    //   spdlog::warn("调整摄像展示区域大小: {},{}->{},{}",
    //                cam_frame_lbl_->size().width(),
    //                cam_frame_lbl_->size().height(), pixmap.size().width(),
    //                pixmap.size().height());
    //   cam_frame_lbl_->setFixedSize(pixmap.size());
    //   cam_frame_lbl_->setMinimumSize(pixmap.size());
    // }
  }
}
