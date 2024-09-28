#include "ela_gui.h"

#include <ElaMessageBar.h>
#include <ElaPushButton.h>
#include <ElaScrollPage.h>
#include <ElaWidget.h>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qobject.h>
#include <qpixmap.h>

#include <opencv2/core/mat.hpp>
#include <spdlog/spdlog.h>

#include "Def.h"
#include "engine.h"
#include "function.h"
#include "gui/register_page.h"

using namespace arm_face_id;

arm_face_id::ElaGUI::ElaGUI() {
  this->setUserInfoCardVisible(false);
  setContentsMargins(0, 0, 0, 0);
  InitWindow();
}

void arm_face_id::ElaGUI::InitWindow() {
  auto w = new ElaScrollPage();

  RegisterPage* register_page = new RegisterPage;
  register_page_ = register_page;

  auto res = addPageNode("注册", register_page, 0, ElaIconType::Book);
  setNavigationBarDisplayMode(
      ElaNavigationType::NavigationDisplayMode::Compact);

  auto detector = FaceDetectorServer::GetInstance();
  auto register_btn = register_page_->findChild<ElaPushButton*>("register_btn");
  if (register_btn)
    QObject::connect(register_btn, &ElaPushButton::clicked, [&, detector]() {
      ElaMessageBar::information(ElaMessageBarType::PositionPolicy::TopRight,
                                 msg_bar_title_, msg_bar_content_, 2000);
      detector->NeedRegisterFace();
    });
}

void arm_face_id::ElaGUI::OnFrameCaptured(cv::Mat frame) {
  if (register_page_) {
    register_page_->setCameraFrame(arm_face_id::utils::mat_to_qimage(frame));
  }
}

void ElaGUI::OnFaceDetected(cv::Mat img, vector<cv::Rect> faces) {
  if (register_page_) {
    register_page_->setCaptureFrame(
        QPixmap::fromImage(utils::mat_to_qimage(img)));
  }
}