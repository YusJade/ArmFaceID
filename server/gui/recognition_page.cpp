#include "recognition_page.h"

#include <ElaToggleSwitch.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include <QHBoxLayout>

#include <spdlog/spdlog.h>

#include "ElaScrollPage.h"
#include "ElaText.h"
#include "engine.h"
#include "face_database.h"

using namespace arm_face_id;

RecognitionPage::RecognitionPage() { InitPage(); }

void RecognitionPage::InitPage() {
  setTitleVisible(false);
  QWidget* central_widget = new QWidget;
  QGridLayout* central_layout = new QGridLayout(central_widget);

  cam_frame_lbl_ = new QLabel;
  cam_frame_lbl_->setMinimumSize(300 * 1.3, 225 * 1.3);

  ElaToggleSwitch* toggle_recognition_btn = new ElaToggleSwitch;
  ElaText* toggle_recognition_text = new ElaText("识别人脸");
  toggle_recognition_text->setMaximumWidth(50);
  toggle_recognition_text->setTextPixelSize(12);
  QHBoxLayout* switch_layout = new QHBoxLayout;
  switch_layout->addWidget(toggle_recognition_text, 1);
  switch_layout->addWidget(toggle_recognition_btn, 4);

  central_layout->addWidget(cam_frame_lbl_, 0, 0);
  central_layout->addLayout(switch_layout, 1, 0);

  setCustomWidget(central_widget);

  QObject::connect(
      toggle_recognition_btn, &ElaToggleSwitch::toggled, this,
      [this](bool toggled) {
        data::User res;
        FaceDetectorServer::GetInstance()->NeedRecognizeFace(toggled);
        // emit toggle_recognition_btn_switched(toggled);
      });
}

void RecognitionPage::setCameraFrame(const QImage& img) {
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

    auto pixmap = ra_pixmap.scaled(cam_frame_lbl_->size(), Qt::KeepAspectRatio);
    cam_frame_lbl_->setPixmap(pixmap);

    // 调整 label 的大小
    if (cam_frame_lbl_->size() != pixmap.size()) {
      spdlog::warn("调整摄像展示区域大小: {},{}->{},{}",
                   cam_frame_lbl_->size().width(),
                   cam_frame_lbl_->size().height(), pixmap.size().width(),
                   pixmap.size().height());
      cam_frame_lbl_->setFixedSize(pixmap.size());
      cam_frame_lbl_->setMinimumSize(pixmap.size());
    }
  }
}