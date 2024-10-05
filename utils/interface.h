#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>

#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

using std::shared_ptr;
using std::vector;

namespace arm_face_id {
namespace interface {

template <typename MsgType>
class FaceDetectorObserver {
 public:
  static const int64_t kFaceNotDetected = -1;
  static const int64_t kFaceAlreadyExisted = -2;

  FaceDetectorObserver() = default;

  virtual void OnFaceDetected(cv::Mat img, vector<cv::Rect> faces) = 0;
  virtual void OnFaceRecognized(cv::Mat img, cv::Rect face, MsgType* id) = 0;

  virtual void OnFaceRegistered(cv::Mat img, cv::Rect face, MsgType* id) {}

 private:
};

/**
 为了兼容已存在的类，特供特化版本 int64_t
 */
template <>
class FaceDetectorObserver<int64_t> {
 public:
  static const int64_t kFaceNotDetected = -1;
  static const int64_t kFaceAlreadyExisted = -2;

  FaceDetectorObserver() = default;

  virtual void OnFaceDetected(cv::Mat img, vector<cv::Rect> faces) = 0;
  virtual void OnFaceRecognized(cv::Mat img, cv::Rect face, int64_t id) = 0;

  virtual void OnFaceRegistered(cv::Mat img, cv::Rect face, int64_t id) {}

 private:
};

template <typename MsgType>
class FaceDetector {
  using MyObserver = FaceDetectorObserver<MsgType>;

 public:
  FaceDetector() = default;

  inline void AddObserver(shared_ptr<MyObserver> observer) {
    if (observer) {
      observers_.push_back(observer);
    }
  }

  inline void removeObserver(shared_ptr<MyObserver> observer) {
    if (observer) {
      auto postion = std::find(observers_.begin(), observers_.end(), observer);
      if (postion != observers_.end()) {
        observers_.erase(postion);
      }
    }
  }

 protected:
  vector<shared_ptr<MyObserver>> observers_;
};

/**
 为了兼容已存在的类，特供特化版本 int64_t
 */
template <>
class FaceDetector<int64_t> {
  using MyObserver = FaceDetectorObserver<int64_t>;

 public:
  FaceDetector() = default;

  inline void AddObserver(shared_ptr<MyObserver> observer) {
    if (observer) {
      observers_.push_back(observer);
    }
  }

  inline void removeObserver(shared_ptr<MyObserver> observer) {
    if (observer) {
      auto postion = std::find(observers_.begin(), observers_.end(), observer);
      if (postion != observers_.end()) {
        observers_.erase(postion);
      }
    }
  }

 protected:
  vector<shared_ptr<MyObserver>> observers_;
};

class CameraObserver {
 public:
  CameraObserver() = default;

  virtual void OnFrameCaptured(cv::Mat frame) = 0;
  virtual void OnCameraShutDown() {};
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

 protected:
  vector<shared_ptr<CameraObserver>> observers_;
};

}  // namespace interface
}  // namespace arm_face_id
