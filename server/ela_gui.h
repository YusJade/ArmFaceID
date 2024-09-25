#include <ElaWindow.h>

#include "interface.h"

namespace arm_face_id {
class ElaGUI : public ElaWindow,
               public interface::FaceDetectorObserver,
               public interface::CameraObserver {
 public:
  ElaGUI();

  void InitWindow();

  virtual void OnFaceDetected(cv::Mat img, vector<cv::Rect> faces) {}
  virtual void OnFaceRecognized(cv::Mat img, cv::Rect face, int64_t id) {}

  virtual void OnFrameCaptured(cv::Mat frame) {};
  virtual void OnCameraShutDown() {};
};

}  // namespace arm_face_id
