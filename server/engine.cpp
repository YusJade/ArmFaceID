
#include "engine.h"

#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <fmt/core.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <seeta/CStruct.h>
#include <spdlog/spdlog.h>

#include "face_engine.h"
#include "function.h"
#include "interface.h"

using namespace arm_face_id;

std::shared_ptr<FaceDetectorServer> FaceDetectorServer::_instance;

std::shared_ptr<FaceDetectorServer> FaceDetectorServer::GetInstance() {
  ASSERET_WITH_LOG("不存在一个人脸检测识别实例！", _instance);
  return _instance;
}

std::shared_ptr<FaceDetectorServer> FaceDetectorServer::BuildAndReturn(
    const Settings& settings) {
  ASSERET_WITH_LOG("已存在一个人脸检测识别实例！", !_instance);
  _instance =
      std::shared_ptr<FaceDetectorServer>(new FaceDetectorServer(settings));
  return _instance;
}

FaceDetectorServer::FaceDetectorServer(const Settings& config)
    : face_engine_(std::make_unique<seeta::FaceEngine>(
          config.fd_setting, config.pd_setting, config.fr_setting)) {
  ASSERET_WITH_LOG("无法加载级联分类器！",
                   classifier_.load(config.classifier_path));
  spdlog::info("已初始化人脸检测识别模块 ~");
  // spdlog::info(cv::getBuildInformation());
}

void FaceDetectorServer::Start() {
  if (worker_thread_ != nullptr) {
    spdlog::warn("当前人脸检测识别实例已存在一个进程~");
    return;
  }
  is_thread_running_ = true;
  worker_thread_ = std::make_unique<std::thread>([&, this] {
    cv::Mat cur_frame;
    while (is_thread_running_) {
      if (frame_queue_.empty()) continue;
      cur_frame = frame_queue_.front();
      frame_queue_.pop();
      std::vector<cv::Rect> faces;
      DetectFace(faces, cur_frame);
      if (faces.empty()) {
        continue;
      }

      spdlog::info("检测到 {} 张人脸 :O", faces.size());
      for (auto iter : this->observers_) {
        iter->OnFaceDetected(cur_frame, faces);
      }

      if (need_register_) {
        RegisterFace(cur_frame);
        need_register_ = false;
      }
    }
    spdlog::info("人脸检测识别进程已退出 ~");
  });
  worker_thread_->detach();
  spdlog::info("人脸检测识别进程已启动");
}

void arm_face_id::FaceDetectorServer::DetectFace(std::vector<cv::Rect>& faces,
                                                 const cv::Mat& frame) {
  cv::Mat gray_frame;
  cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
  cv::equalizeHist(gray_frame, gray_frame);
  classifier_.detectMultiScale(frame, faces);
}

int64_t arm_face_id::FaceDetectorServer::RecognizeFace(const cv::Mat& frame) {
  std::lock_guard<std::mutex> lock_guard(mutex_);
  SeetaImageData img_date{frame.cols, frame.rows, frame.channels(), frame.data};
  float similarity = .0;
  int64_t id = -2;
  id = face_engine_->Query(img_date, &similarity);
  if (id == -2) {
    spdlog::error(
        "Inner Error! :< \n"
        "\t > image: width-{}, height-{}, channels-{}",
        img_date.width, img_date.height, img_date.channels);
    return -2;
  }

  if (id == -1) {
    spdlog::info(
        "Failed to recognize any faces from image :< \n"
        "\t > image: width-{}, height-{}, channels-{}",
        img_date.width, img_date.height, img_date.channels);
  } else if (similarity > 0.5) {
    spdlog::info(
        "Recognize face (id={}, similarity={}) from image :> \n"
        "\t > image: width-{}, height-{}, channels-{}",
        id, similarity, img_date.width, img_date.height, img_date.channels);
  } else {
    spdlog::info(
        "Failed to recognize a properly face (id={}, similarity={}) from image "
        ":< \n"
        "\t > image: width-{}, height-{}, channels-{}",
        id, similarity, img_date.width, img_date.height, img_date.channels);
  }
  return id;
}

int64_t arm_face_id::FaceDetectorServer::RegisterFace(const cv::Mat& frame) {
  SeetaImageData img_date{frame.cols, frame.rows, frame.channels(), frame.data};
  float similarity = .0;
  auto id = face_engine_->Query(img_date, &similarity);
  if (id != -1 && similarity > 0.5) {
    spdlog::info("注册失败: 人脸的注册信息已经存在 :< (id={}, similarity={})",
                 id, similarity);

    for (auto iter : observers_) {
      iter->OnFaceRegistered(
          frame, cv::Rect(),
          interface::FaceDetectorObserver::kFaceAlreadyExisted);
    }

    return id;
  }
  id = face_engine_->Register(img_date);
  if (id == -1) {
    spdlog::info("注册失败: 检测不到人脸 :< \n");
    for (auto iter : observers_) {
      iter->OnFaceRegistered(frame, cv::Rect(),
                             interface::FaceDetectorObserver::kFaceNotDetected);
    }
  } else {
    spdlog::info("注册成功: id= {} :> \n", id);
    for (auto iter : observers_) {
      iter->OnFaceRegistered(frame, cv::Rect(), id);
    }
  }
  return id;
}

bool arm_face_id::FaceDetectorServer::Save(std::string path) {
  bool is_success = false;
  is_success = face_engine_->Save(path.c_str());

  if (is_success) {
    spdlog::info("Saved seetaface database: {}", path);
  } else {
    spdlog::error("Failed to save seetaface database: {}", path);
  }

  return is_success;
}

bool arm_face_id::FaceDetectorServer::Load(std::string path) {
  bool is_success = false;
  is_success = face_engine_->Load(path.c_str());

  if (is_success) {
    spdlog::info("Loaded seetaface database: {}", path);
  } else {
    spdlog::error("Failed to load seetaface database: {}", path);
  }

  return is_success;
}

void FaceDetectorServer::OnCameraShutDown() {}

void FaceDetectorServer::OnFrameCaptured(cv::Mat frame) {
  frame_queue_.push(frame);
  return;
}