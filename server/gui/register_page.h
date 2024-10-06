#pragma once

#include <ElaScrollPage.h>
#include <qimage.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qtmetamacros.h>

#include "ElaPromotionCard.h"

namespace arm_face_id {
class RegisterPage : public ElaScrollPage {
  Q_OBJECT
 public:
  RegisterPage();

  void setCaptureFrame(const QPixmap& img);
  void setCameraFrame(const QImage& img);

 signals:
  void into_register_state();

 public slots:
  void update_capture_card_img(const QPixmap&) {}

 private:
  void InitPage();
  QLabel* cam_frame_lbl_ = nullptr;
  ElaPromotionCard* cap_frame_card_ = nullptr;
};
}  // namespace arm_face_id
