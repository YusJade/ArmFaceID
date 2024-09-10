#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/videoio.hpp>
#include <seeta/Struct.h>

#include "face_engine.h"

namespace arm_face_id {

struct EngineConfig {
  // used to init seeta::FaceEngine.
  seeta::ModelSetting fd_setting;
  seeta::ModelSetting pd_setting;
  seeta::ModelSetting fr_setting;
  // used to init cv::VideoCapture.
  std::string network_camera_url;
  int native_camera_index = -1;
  // the model path which loaded by cv::CascadeClassifie.
  std::string classifier_path;
};

class ICamera {
 public:
  virtual void OnFrameCaptured(cv::Mat) = 0;
};

/**
 * @brief This Engine provide face detection,face recognintion,face register,and
 * persistence of related data, which will be injected into the objects depend
 * the engine.
 */
class Engine {
 public:
  explicit Engine(const EngineConfig &);
  Engine() = delete;
  Engine(const Engine &) = delete;

  // use seeta::FaceEngine
  int64_t RecognizeFace(const cv::Mat &);
  int64_t RegisterFace(const cv::Mat &);

  // use cv::CascadeClassifier
  void DetectFace(std::vector<cv::Rect> &, const cv::Mat &);

  inline void RegisterICamera(std::shared_ptr<ICamera> icamera) {
    icameras_.push_back(icamera);
  }

 private:
  std::unique_ptr<seeta::FaceEngine> face_engine_;
  std::vector<std::shared_ptr<ICamera>> icameras_;
  cv::CascadeClassifier classifier_;
  cv::VideoCapture camera_;

  inline void InvokeAllICamera(const cv::Mat &frame) {
    for (auto i : icameras_) {
      i->OnFrameCaptured(frame);
    }
  }
};
}  // namespace arm_face_id
