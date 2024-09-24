

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

using std::shared_ptr;
using std::vector;

namespace arm_face_id {
namespace interface {

class FaceDetectorObserver {
 public:
  FaceDetectorObserver() = default;

  virtual void OnFaceDetected(cv::Mat img, vector<cv::Rect> faces) = 0;
  virtual void OnFaceRecognized(cv::Mat img, cv::Rect face, int64_t id) = 0;

 private:
};

class FaceDetector {
 public:
  FaceDetector() = default;

  inline void AddObserver(shared_ptr<FaceDetectorObserver> observer) {
    if (observer) {
      observers_.push_back(observer);
    }
  }

  inline void removeObserver(shared_ptr<FaceDetectorObserver> observer) {
    if (observer) {
      auto postion = std::find(observers_.begin(), observers_.end(), observer);
      if (postion != observers_.end()) {
        observers_.erase(postion);
      }
    }
  }

 private:
  vector<shared_ptr<FaceDetectorObserver>> observers_;
};

class CameraObserver {
 public:
  CameraObserver() = default;

  virtual void OnFrameCaptured(cv::Mat frame) = 0;
  virtual void OnCameraShutDown(){};
};

class Camera {
 public:
  Camera() = default;

  inline void AddObserver(shared_ptr<CameraObserver> observer) {
    if (observer) {
      observers_.push_back(observer);
    }
  }

  inline void removeObserver(shared_ptr<CameraObserver> observer) {
    if (observer) {
      auto postion = std::find(observers_.begin(), observers_.end(), observer);
      if (postion != observers_.end()) {
        observers_.erase(postion);
      }
    }
  }

 private:
  vector<shared_ptr<CameraObserver>> observers_;
};

}  // namespace interface
}  // namespace arm_face_id