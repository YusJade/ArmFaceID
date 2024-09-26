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
#include "ElaScrollPage.h"
#include "ElaWidget.h"
#include "function.h"

using namespace arm_face_id;

RegisterPage::RegisterPage() { InitPage(); }

void RegisterPage::InitPage() {
  setPageTitleSpacing(1);
  setTitleVisible(false);
  // cv::Mat mat = cv::imread("./server/assets/test.png");
  // QVBoxLayout* left_card_layout = new QVBoxLayout;
  ElaPromotionCard *image_card = new ElaPromotionCard();
  // // image_card->setFixedSize(QSize(100, 100));
  // image_card->setCardPixmap(QPixmap::fromImage(utils::mat_to_qimage(mat)));
  // left_card_layout->addWidget(new ElaText("保持人脸在画面正中央 :O"), 0);
  // left_card_layout->addWidget(image_card, 10);

  // QVBoxLayout* right_op_layout = new QVBoxLayout;
  // ElaPushButton* register_btn = new ElaPushButton("register");
  // ElaPushButton* test_btn = new ElaPushButton("register");

  // right_op_layout->addWidget(register_btn);
  // right_op_layout->addWidget(test_btn);

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
  camera_label->setStyleSheet(
      "border-radius: 20px; "
      "background-position: center;");
  // cameraLabel->setStyleSheet(
  //     "background-color: #f3f4f6;"
  //     "color: #1f2937;"
  //     "font-size: 16px;"
  //     "padding: 2px;");
  // cameraLabel->setFixedSize();
  // cameraLabel->setFixedSize(600, 450);
  cam_frame_lbl_ = camera_label;
  //   ElaPushButton *captureButton = new ElaPushButton("Capture",
  //   cameraWidget); captureButton->setStyleSheet(
  //       "background-color: #3b82f6; color: #ffffff; padding: 10px 20px; "
  //       "border-radius: 8px;");

  // cameraLayout->addWidget(cameraLabel);
  //   cameraLayout->addWidget(captureButton, 0, Qt::AlignCenter);

  // 用户注册表单
  // QWidget *form_widget = new QWidget(this);
  QVBoxLayout *form_layout = new QVBoxLayout();

  // form_layout->setContentsMargins(2, 0, 2, 0);
  // form_widget->setContentsMargins(0, 0, 0, 0);
  // form_layout->setAlignment(Qt::AlignCenter);

  // QHBoxLayout *mid_layout_in_form = new QHBoxLayout(form_widget);
  // mid_layout_in_form->addSpacing(2);
  // mid_layout_in_form->addWidget(cap_frame_card_);
  // mid_layout_in_form->addSpacing(2);

  // cap_frame_card_ = new ElaPromotionCard(form_widget);
  // cap_frame_card_->setFixedSize(250, 250);
  ElaLineEdit *name_input = new ElaLineEdit;
  name_input->setPlaceholderText("输入你的昵称");
  name_input->setFixedHeight(40);
  name_input->setMinimumSize(120, 30);
  ElaLineEdit *email_input = new ElaLineEdit;
  email_input->setPlaceholderText("输入你的邮箱");
  email_input->setFixedHeight(40);
  name_input->setMinimumSize(120, 30);
  ElaPushButton *register_button = new ElaPushButton("注册");

  form_layout->addWidget(name_input);
  form_layout->addWidget(email_input);
  form_layout->addWidget(register_button);

  main_layout->setContentsMargins(5, 5, 5, 5);
  main_layout->addWidget(cam_frame_lbl_, 0, 0, 2, 1);
  main_layout->addWidget(image_card, 0, 2, 0, 0);
  main_layout->addLayout(form_layout, 1, 2);
  // main_layout->addWidget(image_card, 0, 2);

  // formLayout->addWidget(cap_frame_card_);
  // form_layout->addSpacerItem(new QSpacerItem(10, 10));
  // form_layout->addWidget(name_input);
  // form_layout->addWidget(email_input);
  // form_layout->addWidget(register_button);
  // form_layout->addLayout(mid_layout_in_form);

  // 添加到主布局
  // main_layout->addWidget(camera_widget, 2);
  // main_layout->addWidget(form_widget, 1);
  // main_layout->setSpacing(0);

  // 设置窗口属性
  this->setCustomWidget(central_widget);
  // setLayout(mainLayout);
  //   setWindowTitle("Face Registration");
  //   setStyleSheet("background-color: #ffffff;");
}

void RegisterPage::setCameraFrame(const QImage &img) {
  if (cam_frame_lbl_) {
    QPixmap dest_pixmap(img.size());
    dest_pixmap.fill(Qt::transparent);
    QPainterPath path;
    path.addRoundedRect(img.rect(), 15, 15);
    QPainter painter(&dest_pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setClipPath(path);
    painter.drawPixmap(img.rect(), QPixmap::fromImage(img));
    cam_frame_lbl_->setPixmap(dest_pixmap);
  }
}