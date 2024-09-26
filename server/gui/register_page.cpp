#include "register_page.h"

#include <ElaImageCard.h>
#include <ElaText.h>
#include <qboxlayout.h>
#include <qimage.h>
#include <qsize.h>
#include <qwidget.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include "ElaPushButton.h"
#include "ElaScrollPage.h"
#include "function.h"

using namespace arm_face_id;

RegisterPage::RegisterPage() { InitPage(); }

void RegisterPage::InitPage() {
  setPageTitleSpacing(1);
  setTitleVisible(false);

  cv::Mat mat = cv::imread("./server/assets/test.png");
  cv::imshow("test", mat);
  QVBoxLayout* left_card_layout = new QVBoxLayout;
  ElaImageCard* image_card = new ElaImageCard();
  image_card->setFixedSize(QSize(100, 100));
  image_card->setCardImage(utils::mat_to_qimage(mat));
  left_card_layout->addWidget(new ElaText("保持人脸在画面正中央 :O"));
  left_card_layout->addWidget(image_card);

  QVBoxLayout* right_op_layout = new QVBoxLayout;
  ElaPushButton* register_btn = new ElaPushButton("register");
  ElaPushButton* test_btn = new ElaPushButton("register");

  right_op_layout->addWidget(register_btn);
  right_op_layout->addWidget(test_btn);

  QWidget* central_widget = new QWidget;
  QHBoxLayout* central_layout = new QHBoxLayout(central_widget);
  central_layout->addLayout(left_card_layout, 2);

  addCentralWidget(central_widget);
}
