#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/video.hpp>
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

// NOTE: This listener only be invoked by engine`s worker thread.
class IEngineListener {
 public:
  virtual void OnFaceRecognized(int64_t, const cv::Mat &) = 0;
  virtual void OnFaceDetected(const std::vector<cv::Rect> &,
                              const cv::Mat &) = 0;
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

  void Start();

  // use seeta::FaceEngine
  int64_t RecognizeFace(const cv::Mat &);
  int64_t RegisterFace(const cv::Mat &);

  // use cv::CascadeClassifier
  void DetectFace(std::vector<cv::Rect> &, const cv::Mat &);

  inline void RegisterICamera(std::shared_ptr<ICamera> icamera) {
    icameras_.push_back(icamera);
  }

  inline void RegisterIListener(std::shared_ptr<IEngineListener> ilistener) {
    ilisteners_.push_back(ilistener);
  }

 private:
  std::unique_ptr<seeta::FaceEngine> face_engine_;
  std::unique_ptr<std::thread> worker_thread_;
  std::vector<std::shared_ptr<ICamera>> icameras_;
  std::vector<std::shared_ptr<IEngineListener>> ilisteners_;
  cv::CascadeClassifier classifier_;
  cv::VideoCapture camera_;
  std::mutex mutex_;

  inline void InvokeAllICamera(const cv::Mat &frame) {
    for (auto i : icameras_) {
      i->OnFrameCaptured(frame);
    }
  }

  inline void InvokeAllOnFaceDetected(const std::vector<cv::Rect> &faces,
                                      const cv::Mat &frame) {
    for (auto i : ilisteners_) {
      i->OnFaceDetected(faces, frame);
    }
  }

  inline void InvokeAllOnFaceRecognized(int64_t id, const cv::Mat &frame) {
    for (auto i : ilisteners_) {
      i->OnFaceRecognized(id, frame);
    }
  }

  // inline void InvokeAllIListeners(std::v)
};
}  // namespace arm_face_id
