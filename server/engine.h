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
#include "interface.h"

namespace arm_face_id {

class [[deprecated]] ICamera {
 public:
  virtual void OnFrameCaptured(cv::Mat) = 0;
};

class [[deprecated]] IEngineListener {
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
class FaceDetectorServer : public interface::FaceDetector,
                           public interface::CameraObserver {
 public:
  struct Settings {
    // 初始化 seeta::FaceEngine 在人脸检测模块
    seeta::ModelSetting fd_setting;
    // 初始化 seeta::FaceEngine 在人脸检测模块
    seeta::ModelSetting pd_setting;
    // 初始化 seeta::FaceEngine 在人脸识别模块
    seeta::ModelSetting fr_setting;

    // OpenCV 级联分类器在路径
    std::string classifier_path;
  };
  explicit FaceDetectorServer(const Settings &);
  FaceDetectorServer() = delete;
  FaceDetectorServer(const FaceDetectorServer &) = delete;

  void Start();

  // use seeta::FaceEngine
  int64_t RecognizeFace(const cv::Mat &);
  int64_t RegisterFace(const cv::Mat &);
  // use cv::CascadeClassifier
  void DetectFace(std::vector<cv::Rect> &, const cv::Mat &);

  bool Save(std::string path);
  bool Load(std::string path);

  virtual void OnCameraShutDown() override;
  virtual void OnFrameCaptured(cv::Mat frame) override;

  [[deprecated]] void RegisterICamera(std::shared_ptr<ICamera> icamera) {
    icameras_.push_back(icamera);
  }

  [[deprecated]]
  void RegisterIListener(std::shared_ptr<IEngineListener> ilistener) {
    ilisteners_.push_back(ilistener);
  }

 private:
  std::unique_ptr<seeta::FaceEngine> face_engine_;
  std::unique_ptr<std::thread> worker_thread_;
  cv::CascadeClassifier classifier_;
  std::mutex mutex_;

  [[deprecated]]
  cv::VideoCapture camera_;
  [[deprecated]]
  std::vector<std::shared_ptr<ICamera>> icameras_;
  [[deprecated]]
  std::vector<std::shared_ptr<IEngineListener>> ilisteners_;

  [[deprecated]]
  inline void InvokeAllICamera(const cv::Mat &frame) {
    for (auto i : icameras_) {
      i->OnFrameCaptured(frame);
    }
  }

  [[deprecated]]
  inline void InvokeAllOnFaceDetected(const std::vector<cv::Rect> &faces,
                                      const cv::Mat &frame) {
    for (auto i : ilisteners_) {
      i->OnFaceDetected(faces, frame);
    }
  }

  [[deprecated]]
  inline void InvokeAllOnFaceRecognized(int64_t id, const cv::Mat &frame) {
    for (auto i : ilisteners_) {
      i->OnFaceRecognized(id, frame);
    }
  }

  // inline void InvokeAllIListeners(std::v)
};
}  // namespace arm_face_id
