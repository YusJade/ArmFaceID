#include "camera.h"

#include <string>

#include <absl/log/absl_log.h>

bool arm_face_id::Camera::Open(int native_camera_index) {
  bool is_opened = capture_.open(native_camera_index);
  is_ready_running_ = is_opened;
  return is_opened;
}

bool arm_face_id::Camera::Open(std::string network_camera_url) {
  bool is_opened = capture_.open(network_camera_url);
  is_ready_running_ = is_opened;
  return is_opened;
}

void arm_face_id::Camera::Start() {
  if (!is_ready_running_) {
    ABSL_LOG(WARNING) << "Camera not opened!";
  }
  cv::Mat cur_frame;
  while (is_ready_running_) {
    capture_ >> cur_frame;
    if (!cur_frame.empty()) {
      InvokeAllListener(cur_frame);
    }
  }
  ABSL_LOG(INFO) << "Camera stopped.";
}