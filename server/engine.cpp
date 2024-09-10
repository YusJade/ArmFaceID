
#include "engine.h"

#include <cstdint>
#include <memory>
#include <vector>

#include <fmt/core.h>
#include <opencv2/core/types.hpp>
#include <seeta/CStruct.h>
#include <spdlog/spdlog.h>

#include "face_engine.h"
#include "utils/utils.h"

arm_face_id::Engine::Engine(const EngineConfig& config)
    : face_engine_(std::make_unique<seeta::FaceEngine>(
          config.fd_setting, config.pd_setting, config.fr_setting)) {
  ASSERET_WITH_LOG("Failed to load model of Calssifier.",
                   classifier_.load(config.classifier_path));
  spdlog::info("Initialized the Engine.");
  if (!config.network_camera_url.empty())
    camera_.open(config.network_camera_url);
  if (camera_.isOpened()) {
    spdlog::info("Opened network camera at {}", config.network_camera_url);
    return;
  }
  spdlog::warn("Failed to open network camera at {}, please check:<",
               config.network_camera_url);
  camera_.open(config.native_camera_index);
  if (camera_.isOpened()) {
    spdlog::info("Open native camera device {}", config.native_camera_index);
    return;
  }
  ASSERET_WITH_LOG(
      fmt::format(fmt::format_string<int>(
                      "Failed to open native camera at {}, please check:<"),
                  config.native_camera_index),
      camera_.isOpened())
}

void arm_face_id::Engine::DetectFace(std::vector<cv::Rect>& faces,
                                     const cv::Mat& frame) {
  classifier_.detectMultiScale(frame, faces);
}

int64_t arm_face_id::Engine::RecognizeFace(const cv::Mat& frame) {
  SeetaImageData img_date{frame.cols, frame.rows, frame.channels(), frame.data};
  return face_engine_->Query(img_date);
}

int64_t arm_face_id::Engine::RegisterFace(const cv::Mat& frame) {
  SeetaImageData img_date{frame.cols, frame.rows, frame.channels(), frame.data};
  return face_engine_->Register(img_date);
}
