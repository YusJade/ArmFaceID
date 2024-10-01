#pragma once

#include <ElaScrollPage.h>
#include <qimage.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qtmetamacros.h>

namespace arm_face_id {
class RecognitionPage : public ElaScrollPage {
 public:
  RecognitionPage();

  void setCameraFrame(const QImage& img);

 private:
  void InitPage();
  QLabel* cam_frame_lbl_ = nullptr;
};

}  // namespace arm_face_id
