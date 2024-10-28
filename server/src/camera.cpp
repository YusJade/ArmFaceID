#include "camera.h"

#include <thread>

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <spdlog/spdlog.h>

using namespace arm_face_id;

Camera::Camera(const Camera::Settings& settings,
               treasure_chest::pattern::SyncQueue<cv::Mat>& queue)
    : treasure_chest::pattern::Producer<cv::Mat>(queue), settings_(settings) {
  spdlog::info(
      "摄像头设置: [cam_url={}, cam_index={}, "
      "enable_net_cam={}].",
      settings.cam_url, settings.cam_index, settings.enable_net_cam);
}

void Camera::Start() {
  if (!cap_.isOpened()) {
    spdlog::error("无法打开摄像头");
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
      spdlog::warn("无法打开网络摄像头，检查设置的 url :< [cam_url:{}]",
                   settings_.cam_url);
    }
  } else {
    spdlog::info("网络摄像头已被禁用 :O [enable_net_cam:]",
                 settings_.enable_net_cam);
  }

  if (cap_.isOpened()) {
    spdlog::info("成功打开网络摄像头 :) [cam_url:{}]", settings_.cam_url);
  } else if (cap_.open(settings_.cam_index)) {
    spdlog::info("成功打开本地摄像头 :) [cam_index:{}]", settings_.cam_index);
  } else {
    spdlog::error("无法打开摄像头 :( [cam_index:{}]", settings_.cam_index);
    return false;
  }

  return true;
}
