#include "register_page.h"

#include <ElaImageCard.h>
#include <ElaLineEdit.h>
#include <ElaPromotionCard.h>
#include <ElaText.h>
#include <qboxlayout.h>
#include <qgridlayout.h>
#include <qimage.h>
#include <qlabel.h>
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

#include "ElaPushButton.h"

using namespace arm_face_id;

RegisterPage::RegisterPage() { InitPage(); }

void RegisterPage::InitPage() {
  setPageTitleSpacing(1);
  setTitleVisible(false);
  ElaPromotionCard *image_card = new ElaPromotionCard();

  QWidget *central_widget = new QWidget;
  QGridLayout *main_layout = new QGridLayout(central_widget);
  main_layout->setContentsMargins(0, 0, 0, 0);
  main_layout->setAlignment(Qt::AlignLeft);
  central_widget->setContentsMargins(0, 0, 0, 0);
  // 摄像头捕捉区域

  QWidget *camera_widget = new QWidget(this);
  QVBoxLayout *cameraLayout = new QVBoxLayout(camera_widget);
  cameraLayout->setContentsMargins(0, 0, 0, 0);
  camera_widget->setContentsMargins(0, 0, 0, 0);

  QLabel *camera_label = new QLabel(camera_widget);
  camera_label->setMinimumSize(300, 225);
  cam_frame_lbl_ = camera_label;

  // 用户注册表单
  // QWidget *form_widget = new QWidget(this);
  QVBoxLayout *form_layout = new QVBoxLayout();

  cap_frame_card_ = image_card;
  // cap_frame_card_->setFixedSize(250, 250);
  ElaLineEdit *name_input = new ElaLineEdit;
  name_input->setPlaceholderText("输入你的昵称");
  name_input->setFixedHeight(30);
  name_input->setMinimumSize(120, 30);
  // name_input->setMaximumSize(120, 30);
  ElaLineEdit *email_input = new ElaLineEdit;
  email_input->setPlaceholderText("输入你的邮箱");
  email_input->setFixedHeight(30);
  // name_input->setFixedSize(120, 30);
  ElaPushButton *register_button = new ElaPushButton("注册");
  register_button->setObjectName("register_btn");

  form_layout->addWidget(name_input);
  form_layout->addWidget(email_input);
  form_layout->addWidget(register_button);

  main_layout->setContentsMargins(2, 2, 2, 2);
  main_layout->addWidget(cam_frame_lbl_, 0, 0, 2, 1);
  // main_layout->addWidget(image_card, 0, 2);
  main_layout->addLayout(form_layout, 2, 0, 2, 2);

  // 设置窗口属性
  this->setCustomWidget(central_widget);

  connect(register_button, &ElaPushButton::clicked,
          [this] { emit register_btn_clicked(); });
}

void RegisterPage::setCaptureFrame(const QPixmap &img) {
  if (cap_frame_card_) {
    cap_frame_card_->setCardPixmap(img);
  }
}

void RegisterPage::setCameraFrame(const QImage &img) {
  if (cam_frame_lbl_) {
    QPixmap dest_pixmap(img.size());
    dest_pixmap.fill(Qt::transparent);
    QPainterPath path;
    path.addRoundedRect(img.rect(), 40, 40);
    QPainter painter(&dest_pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setClipPath(path);
    painter.drawPixmap(img.rect(), QPixmap::fromImage(img));
    cam_frame_lbl_->setPixmap(dest_pixmap);
  }
}
