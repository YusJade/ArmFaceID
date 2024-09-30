#include "ela_gui.h"

#include <ElaMessageBar.h>
#include <ElaPushButton.h>
#include <ElaScrollPage.h>
#include <ElaWidget.h>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qtmetamacros.h>

#include <cstdint>

#include <opencv2/core/mat.hpp>
#include <spdlog/spdlog.h>

#include "Def.h"
#include "engine.h"
#include "function.h"
#include "gui/recognition_page.h"
#include "gui/register_page.h"
#include "interface.h"

using namespace arm_face_id;

arm_face_id::ElaGUI::ElaGUI() {
  this->setUserInfoCardVisible(false);
  setContentsMargins(0, 0, 0, 0);
  InitWindow();
  setFixedSize(400, 325);
  // adjustSize();
}

void arm_face_id::ElaGUI::InitWindow() {
  auto w = new ElaScrollPage();

  register_page_ = new RegisterPage;
  recognition_page_ = new RecognitionPage;

  auto res = addPageNode("注册", register_page_, 0, ElaIconType::Book);
  setNavigationBarDisplayMode(
      ElaNavigationType::NavigationDisplayMode::Compact);

  res = addPageNode("识别", recognition_page_, 0, ElaIconType::FaceWorried);
  setNavigationBarDisplayMode(
      ElaNavigationType::NavigationDisplayMode::Compact);

  auto detector = FaceDetectorServer::GetInstance();
  auto register_btn = register_page_->findChild<ElaPushButton*>("register_btn");
  if (register_btn) {
    QObject::connect(register_btn, &ElaPushButton::clicked,
                     [&, detector, register_btn]() {
                       msg_bar_content_ = "注册中，稍后~";
                       msg_bar_title_ = "注册";
                       ElaMessageBar::information(
                           ElaMessageBarType::PositionPolicy::TopRight,
                           msg_bar_title_, msg_bar_content_, 3000);
                       detector->NeedRegisterFace();
                       register_btn->setDisabled(true);
                     });
  }
  QObject::connect(
      this, &ElaGUI::face_detector_event, this, [&, register_btn](int64_t id) {
        if (register_btn) {
          register_btn->setDisabled(false);
        }
        switch (id) {
          case interface::FaceDetectorObserver::kFaceNotDetected:
            msg_bar_content_ = "请确保人脸在摄像区域内！:(";
            msg_bar_title_ = "错误";
            ElaMessageBar::error(ElaMessageBarType::PositionPolicy::TopRight,
                                 msg_bar_title_, msg_bar_content_, 3000);
            break;
          case interface::FaceDetectorObserver::kFaceAlreadyExisted:
            msg_bar_content_ = "人脸已存在！:P";
            msg_bar_title_ = "错误";
            ElaMessageBar::error(ElaMessageBarType::PositionPolicy::TopRight,
                                 msg_bar_title_, msg_bar_content_, 3000);
            break;
          default:
            msg_bar_content_ = "人脸注册成功！:>";
            msg_bar_title_ = "完成";
            ElaMessageBar::success(ElaMessageBarType::PositionPolicy::TopRight,
                                   msg_bar_title_, msg_bar_content_, 3000);
        }
      });
}

void arm_face_id::ElaGUI::OnFrameCaptured(cv::Mat frame) {
  auto qimg = arm_face_id::utils::mat_to_qimage(frame);
  if (register_page_) {
    register_page_->setCameraFrame(qimg);
  }
  if (recognition_page_) {
    recognition_page_->setCameraFrame(qimg);
  }
}

void ElaGUI::OnFaceDetected(cv::Mat img, vector<cv::Rect> faces) {
  if (register_page_) {
    register_page_->setCaptureFrame(
        QPixmap::fromImage(utils::mat_to_qimage(img)));
  }
}

void ElaGUI::OnFaceRegistered(cv::Mat img, cv::Rect face, int64_t id) {
  emit face_detector_event(id);
}