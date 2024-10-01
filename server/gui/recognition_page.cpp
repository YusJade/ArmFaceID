#include "recognition_page.h"

#include <qgridlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qwidget.h>

using namespace arm_face_id;

RecognitionPage::RecognitionPage() { InitPage(); }

void RecognitionPage::InitPage() {
  setTitleVisible(false);
  QWidget* central_widget = new QWidget;
  QGridLayout* central_layout = new QGridLayout(central_widget);

  cam_frame_lbl_ = new QLabel;
  central_layout->addWidget(cam_frame_lbl_, 1, 1);

  setCustomWidget(central_widget);
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
    cam_frame_lbl_->setFixedSize(pixmap.size());
  }
}