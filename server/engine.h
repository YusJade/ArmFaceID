#pragma once

#include <memory>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <seeta/Struct.h>

#include "face_engine.h"

namespace arm_face_id {

struct EngineConfig {
  seeta::ModelSetting fd_setting;
  seeta::ModelSetting pd_setting;
  seeta::ModelSetting fr_setting;
  std::string network_camera_url;
  int native_camera_index = -1;
};

class ICamera {
 public:
  virtual void OnFrameCaptured(cv::Mat) = 0;
};

/**
Engine 提供人脸识别、检测功能、以及数据持久化、注册摄像头回调的功能
 */
class Engine {
 public:
  explicit Engine(const EngineConfig&);
  Engine() = delete;
  Engine(const Engine&) = delete;

  int64_t RecognizeFace(cv::Mat);

  inline void RegisterICamera(std::shared_ptr<ICamera> icamera) {
    icameras_.push_back(icamera);
  }

 private:
  std::unique_ptr<seeta::FaceEngine> face_engine_;
  std::vector<std::shared_ptr<ICamera>> icameras_;

  inline void InvokeAllICamera(const cv::Mat& frame) {
    for (auto i : icameras_) {
      i->OnFrameCaptured(frame);
    }
  }
};
}  // namespace arm_face_id