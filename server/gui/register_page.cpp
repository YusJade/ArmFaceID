#include "register_page.h"

#include <ElaImageCard.h>
#include <ElaLineEdit.h>
#include <ElaPromotionCard.h>
#include <ElaText.h>
#include <qboxlayout.h>
#include <qimage.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qsize.h>
#include <qwidget.h>

#include <QLineEdit>

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
  // ElaPromotionCard* image_card = new ElaPromotionCard();
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
  // QHBoxLayout* central_layout = new QHBoxLayout(central_widget);
  // central_layout->addLayout(left_card_layout, 5);
  // central_layout->addLayout(right_op_layout, 2);

  // addCentralWidget(central_widget);
  // 设置窗口布局
  QHBoxLayout *mainLayout = new QHBoxLayout(central_widget);

  // 摄像头捕捉区域

  QWidget *cameraWidget = new QWidget(this);
  QVBoxLayout *cameraLayout = new QVBoxLayout(cameraWidget);

  QLabel *cameraLabel = new QLabel("摄像头捕获", cameraWidget);
  cameraLabel->setStyleSheet(
      "background-color: #f3f4f6;"
      " color: #1f2937;"
      " font-size: 16px;"
      " padding: 10px;");
  cameraLabel->setAlignment(Qt::AlignCenter);
  cameraLabel->setFixedSize(800, 600);

  //   ElaPushButton *captureButton = new ElaPushButton("Capture",
  //   cameraWidget); captureButton->setStyleSheet(
  //       "background-color: #3b82f6; color: #ffffff; padding: 10px 20px; "
  //       "border-radius: 8px;");

  cameraLayout->addWidget(cameraLabel);
  //   cameraLayout->addWidget(captureButton, 0, Qt::AlignCenter);

  // 用户注册表单
  QWidget *formWidget = new QWidget(this);
  QVBoxLayout *formLayout = new QVBoxLayout(formWidget);

  QLabel *titleLabel = new QLabel("----------------------------", formWidget);
  titleLabel->setStyleSheet(
      "font-size: 24px;"
      " font-weight: 600;"
      " color: #1f2937;"
      " margin-bottom: 20px;");

  ElaLineEdit *nameInput = new ElaLineEdit(formWidget);
  nameInput->setPlaceholderText("输入你的昵称");
  nameInput->setStyleSheet(
      //   "background-color: #e5e7eb; "
      //   "color: #1f2937; padding: 10px; "
      "height: 39;"
      "border-radius: 4px; "
      "margin-bottom: 10px;");

  ElaLineEdit *emailInput = new ElaLineEdit(formWidget);
  emailInput->setPlaceholderText("输入你的邮箱");
  emailInput->setStyleSheet(
      //   "background-color: #e5e7eb; "
      //   "color: #1f2937; padding: 10px; "
      "height: 30px;"
      "border-radius: 4px; "
      "margin-bottom: 10px;");

  ElaPushButton *registerButton = new ElaPushButton("注册", formWidget);
  registerButton->setStyleSheet(
      // "background-color: #3b82f6;"
      // " color: #ffffff;"
      "width: 120px;"
      " padding: 20px; "
      "border-radius: 8px;");

  formLayout->addWidget(titleLabel);
  formLayout->addWidget(nameInput);
  formLayout->addWidget(emailInput);
  formLayout->addWidget(registerButton);

  // 添加到主布局
  mainLayout->addWidget(cameraWidget, 5);
  mainLayout->addWidget(formWidget, 3);

  // 设置窗口属性
  this->setCustomWidget(central_widget);
  // setLayout(mainLayout);
  //   setWindowTitle("Face Registration");
  //   setStyleSheet("background-color: #ffffff;");
}
