#pragma once

#include <ElaMessageBar.h>
#include <ElaWindow.h>
#include <qtmetamacros.h>

#include <cstdint>

#include "gui/recognition_page.h"
#include "gui/register_page.h"
#include "interface.h"

namespace arm_face_id {
class ElaGUI : public ElaWindow,
               public interface::FaceDetectorObserver,
               public interface::CameraObserver {
  Q_OBJECT
 public:
  ElaGUI();

  void InitWindow();

  virtual void OnFaceDetected(cv::Mat img, vector<cv::Rect> faces);
  virtual void OnFaceRecognized(cv::Mat img, cv::Rect face, int64_t id);

  virtual void OnFaceRegistered(cv::Mat img, cv::Rect face, int64_t id);

  virtual void OnFrameCaptured(cv::Mat frame);
  virtual void OnCameraShutDown() {};

 signals:
  void face_registered(int64_t id);
  void face_recognized(int64_t id);

 private:
  RegisterPage* register_page_ = nullptr;
  RecognitionPage* recognition_page_ = nullptr;
  QString msg_bar_title_ = "null";
  QString msg_bar_content_ = "null";
};

}  // namespace arm_face_id
