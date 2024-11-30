#include "camera.h"

#include <thread>

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <spdlog/spdlog.h>

using namespace arm_face_id;

Camera::Camera(const Camera::Settings& settings,
               treasure_chest::pattern::SyncQueue<cv::Mat>& queue)
    : treasure_chest::pattern::Producer<cv::Mat>(queue), settings_(settings) {
  SPDLOG_INFO(
      "Camera setting: (cam_url={}, cam_index={}, "
      "enable_net_cam={}).",
      settings.cam_url, settings.cam_index, settings.enable_net_cam);
}

void Camera::Start() {
  if (!cap_.isOpened()) {
    SPDLOG_ERROR("Failed to open camera.");
    return;
  }
  is_running_ = true;
  cv::Mat frame;
  while (is_running_) {
    cap_ >> frame;
    if (frame.empty()) continue;
    // 向任务队列发送任务
    PutTask(frame);
    // 通知观察者新的画面帧
    Notify(frame);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  }
}

bool Camera::Open() {
  // bool is_success = false;
  if (settings_.enable_net_cam) {
    cap_.open(settings_.cam_url);
    if (!cap_.isOpened()) {
      SPDLOG_WARN("无法打开网络摄像头，检查设置的 url :< [cam_url:{}]",
                  settings_.cam_url);
    }
  } else {
    SPDLOG_INFO("网络摄像头已被禁用 :O [enable_net_cam:]",
                settings_.enable_net_cam);
  }

  if (cap_.isOpened()) {
    SPDLOG_INFO("成功打开网络摄像头 :) [cam_url:{}]", settings_.cam_url);
  } else if (cap_.open(settings_.cam_index)) {
    SPDLOG_INFO("成功打开本地摄像头 :) [cam_index:{}]", settings_.cam_index);
  } else {
    SPDLOG_ERROR("无法打开摄像头 :( [cam_index:{}]", settings_.cam_index);
    return false;
  }

  return true;
}
