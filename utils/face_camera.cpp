#include "face_camera.h"

#include <chrono>
#include <memory>
#include <thread>

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <spdlog/spdlog.h>

using namespace arm_face_id;

FaceCamera::FaceCamera(const FaceCamera::Settings& settings)
    : settings_(settings), cap_() {
  spdlog::info(
      "# cam_url={} #\n"
      "# cam_index={} #\n "
      "# enable_net_cam={} #",
      settings.cam_url, settings.cam_index, settings.enable_net_cam);
}

bool FaceCamera::FaceCamera::OpenAndStart() {
  // bool is_success = false;
  if (settings_.enable_net_cam) {
    cap_.open(settings_.cam_url);
    if (!cap_.isOpened()) {
      spdlog::warn("无法打开网络摄像头，检查设置的 url :< [url:{}]",
                   settings_.cam_url);
    }
  } else {
    spdlog::info("网络摄像头已被禁用 :O [enable_net_cam:]",
                 settings_.enable_net_cam);
  }

  if (cap_.isOpened()) {
    spdlog::info("成功打开网络摄像头 :) [url:{}]", settings_.cam_url);
    return true;
  }

  if (cap_.open(settings_.cam_index)) {
    spdlog::info("成功打开本地摄像头 :) [index:{}]", settings_.cam_index);
  } else {
    spdlog::error("无法打开摄像头 :( [index:{}]", settings_.cam_index);
    return false;
  }

  cam_thread_ = std::make_unique<std::thread>([this] {
    is_running_ = true;

    cv::Mat frame;
    while (is_running_) {
      if (!cap_.isOpened()) continue;
      cap_ >> frame;
      if (frame.empty()) continue;

      for (auto iter : observers_) {
        iter->OnFrameCaptured(frame);
      }

      cv::imshow("cap", frame);

      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
  });
  cam_thread_->detach();
  return true;
}