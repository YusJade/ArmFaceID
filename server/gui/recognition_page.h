#pragma once

#include <ElaScrollPage.h>
#include <qimage.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qtmetamacros.h>

namespace arm_face_id {
class RecognitionPage : public ElaScrollPage {
  Q_OBJECT
 public:
  RecognitionPage();

  void setCameraFrame(const QImage& img);

 signals:
  void toggle_recognition_btn_switched(bool);

 private:
  void InitPage();
  QLabel* cam_frame_lbl_ = nullptr;
};

}  // namespace arm_face_id
