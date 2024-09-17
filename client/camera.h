#pragma once

#include <memory>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>

namespace arm_face_id {

class ICamera {
 public:
  virtual void OnFrameCaptured(cv::Mat) = 0;
};

class Camera {
 public:
  Camera() = default;
  bool Open(int native_camera_index);
  bool Open(std::string network_camera_url);
  void Start();
  inline void Stop() { is_ready_running_ = false; }
  void RegisterListener(std::shared_ptr<ICamera> listener) {
    listeners_.push_back(listener);
  }

 private:
  bool is_ready_running_ = true;
  cv::VideoCapture capture_;
  std::vector<std::shared_ptr<ICamera>> listeners_;

  void InvokeAllListener(cv::Mat frame) {
    for (auto i : listeners_) {
      i->OnFrameCaptured(frame);
    }
  }
};

}  // namespace arm_face_id