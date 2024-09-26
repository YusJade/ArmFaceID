#pragma once

#include <ElaScrollPage.h>
#include <qimage.h>
#include <qlabel.h>

#include "ElaPromotionCard.h"

namespace arm_face_id {
class RegisterPage : public ElaScrollPage {
 public:
  RegisterPage();

  void setCameraFrame(const QImage& img);

 private:
  void InitPage();
  QLabel* cam_frame_lbl_ = nullptr;
  ElaPromotionCard* cap_frame_card_ = nullptr;
};
}  // namespace arm_face_id