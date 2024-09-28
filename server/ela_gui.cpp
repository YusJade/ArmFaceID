#include "ela_gui.h"

#include <ElaPushButton.h>
#include <ElaScrollPage.h>
#include <ElaWidget.h>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qpixmap.h>

#include <opencv2/core/mat.hpp>
#include <spdlog/spdlog.h>

#include "Def.h"
#include "function.h"
#include "gui/register_page.h"

using namespace arm_face_id;

arm_face_id::ElaGUI::ElaGUI() {
  this->setUserInfoCardVisible(false);
  setContentsMargins(0, 0, 0, 0);
  InitWindow();
}

void arm_face_id::ElaGUI::InitWindow() {
  // setCustomWidget(ElaAppBarType::CustomArea::LeftArea,
  //                 new QLabel("这是个标签~"));
  auto w = new ElaScrollPage();

  RegisterPage* register_page = new RegisterPage;
  register_page_ = register_page;
  // auto l = new QHBoxLayout(w);
  // w->layout()->addWidget(new QLabel("这是个标签~"));
  // w->layout()->addWidget(new ElaPushButton("注册"));
  // w->setWindowButtonFlag(ElaAppBarType::ButtonType::CloseButtonHint, false);
  auto res = addPageNode("注册", register_page, 0, ElaIconType::Book);
  setNavigationBarDisplayMode(
      ElaNavigationType::NavigationDisplayMode::Compact);
  // spdlog::warn("添加导航栏节点：{}",
  //  register_page->windowTitle().toStdString());
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