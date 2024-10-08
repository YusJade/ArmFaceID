
#include "engine.h"

#include <qdatetime.h>

#include <chrono>
#include <cstdint>
#include <ctime>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <fmt/core.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <seeta/CStruct.h>
#include <spdlog/spdlog.h>

#include "face_database.h"
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
        // TODO

        continue;
      }

      spdlog::info("检测到 {} 张人脸 :O", faces.size());
      for (auto iter : this->observers_) {
        iter->OnFaceDetected(cur_frame, faces);
      }
      if (need_recognize_) {
        RecognizeFaceFromDb(cur_frame);
        // RecognizeFace(cur_frame);
      }
    }
    spdlog::info("人脸检测进程已退出 ~");
  });
  worker_thread_->detach();

  register_thread_ = std::make_unique<std::thread>([&, this] {
    cv::Mat cur_frame;
    while (is_thread_running_) {
      if (register_queue_.empty()) continue;
      cur_frame = register_queue_.front();
      register_queue_.pop();
      // std::vector<cv::Rect> faces;
      // DetectFace(faces, cur_frame);
      // if (faces.empty()) {
      //   // TODO

      //   continue;
      // }
      RegisterFace(cur_frame, next_register_user_);
    }
    spdlog::info("人脸识别进程已退出 ~");
  });
  register_thread_->detach();

  spdlog::info("人脸检测识别进程已启动");
}

void FaceDetectorServer::DetectFace(std::vector<cv::Rect>& faces,
                                    const cv::Mat& frame) {
  cv::Mat gray_frame;
  cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
  cv::equalizeHist(gray_frame, gray_frame);
  classifier_.detectMultiScale(frame, faces);
}

int64_t FaceDetectorServer::RecognizeFace(const cv::Mat& frame) {
  std::lock_guard<std::mutex> lock_guard(mutex_);
  SeetaImageData img_date{frame.cols, frame.rows, frame.channels(), frame.data};
  float similarity = .0;
  int64_t id = -2;
  id = face_engine_->Query(img_date, &similarity);
  if (id == -2) {
    spdlog::error("发生内部错误！ :< ");
    return -2;
  }
  if (id == -1) {
    spdlog::info("无法识别到人脸 :< ");
  } else if (similarity > 0.5) {
    spdlog::info("识别到人脸: id={}, similarity={} :>", id, similarity);
  } else {
    spdlog::info("无法断定人脸: id={}, similarity={} :P", id, similarity);
  }
  for (auto iter : observers_) {
    iter->OnFaceRecognized(frame, cv::Rect(), id);
  }

  cv::imwrite(fmt::format(".log_imgs/{}_face_recog.png",
                          QDateTime::currentDateTime().toString()),
              frame);

  return id;
}

int64_t FaceDetectorServer::RegisterFace(const cv::Mat& frame) {
  SeetaImageData img_date{frame.cols, frame.rows, frame.channels(), frame.data};
  float similarity = .0;
  auto id = face_engine_->Query(img_date, &similarity);
  if (id != -1 && similarity > 0.5) {
    spdlog::info("注册失败: 人脸的注册信息已经存在 :< (id={}, similarity={})",
                 id, similarity);

    for (auto iter : observers_) {
      iter->OnFaceRegistered(
          frame, cv::Rect(),
          interface::FaceDetectorObserver<int64_t>::kFaceAlreadyExisted);
    }

    return interface::FaceDetectorObserver<int64_t>::kFaceAlreadyExisted;
  }
  id = face_engine_->Register(img_date);
  if (id == -1) {
    spdlog::info("注册失败: 检测不到人脸 :< \n");
    for (auto iter : observers_) {
      iter->OnFaceRegistered(
          frame, cv::Rect(),
          interface::FaceDetectorObserver<int64_t>::kFaceNotDetected);
    }
  } else {
    spdlog::info("注册成功: id= {} :> \n", id);

    for (auto iter : observers_) {
      iter->OnFaceRegistered(frame, cv::Rect(), id);
    }
  }
  return id;
}
// TODO: 资源竞争
int64_t FaceDetectorServer::RecognizeFaceFromDb(const cv::Mat& img,
                                                data::User* info) {
  float top_similarity = 0.0;
  data::User user;
  user.id = -1;

  float similarity = 0.0;
  SeetaImageData target_img{img.cols, img.rows, img.channels(), img.data};
  cv::Mat decoded_mat;
  auto& db = data::FaceDataBase::GetInstance();
  for (auto& iter : db.Users()) {
    decoded_mat =
        cv::imdecode(cv::Mat(iter.face_img_bytes), cv::IMREAD_UNCHANGED);
    SeetaImageData db_img{decoded_mat.cols, decoded_mat.rows,
                          decoded_mat.channels(), decoded_mat.data};
    {
      std::lock_guard<std::mutex> guard(mutex_);
      similarity = face_engine_->Compare(target_img, db_img);
    }

    if (similarity > top_similarity) {
      top_similarity = similarity;
      user = iter;
    }
  }

  if (top_similarity < 0.6) {
    return -1;
  } else {
    if (info) *info = user;
    for (auto iter : observers_) {
      iter->OnFaceRecognized(img, cv::Rect(), user.id);
    }
  }

  return user.id;
}

int64_t FaceDetectorServer::RegisterFace(const cv::Mat& _frame,
                                         const data::User& user) {
  cv::Mat frame = _frame.clone();
  auto faces = face_engine_->DetectFaces(
      {frame.cols, frame.rows, frame.channels(), frame.data});
  if (faces.empty()) {
    spdlog::info("注册失败: 检测不到人脸 :< \n");
    for (auto iter : observers_) {
      iter->OnFaceRegistered(
          frame, cv::Rect(),
          interface::FaceDetectorObserver<int64_t>::kFaceNotDetected);
    }
    return interface::FaceDetectorObserver<int64_t>::kFaceNotDetected;
  }
  // frame = cv::imread("./server/assets/test.png");
  // cv::imshow("DB COMP", frame);
  // cv::waitKey();
  auto id = this->RecognizeFaceFromDb(frame);
  if (id == -1) {
    // cv::imshow("DB INS", frame);
    // cv::waitKey();
    // TODO: const cv::Mat& frame 源对象被修改了！！！
    data::FaceDataBase::GetInstance().AddUser(user.nick_name, user.email,
                                              utils::mat_to_qimage(frame));
    // 刷新内存中的用户信息
    data::FaceDataBase::GetInstance().LoadToCache();
    for (auto iter : observers_) {
      iter->OnFaceRegistered(frame, cv::Rect(), 999);
    }
  } else {
    spdlog::info("注册失败: 人脸的注册信息已经存在 :< (id={}, similarity={})",
                 id);
    for (auto iter : observers_) {
      iter->OnFaceRegistered(
          frame, cv::Rect(),
          interface::FaceDetectorObserver<int64_t>::kFaceAlreadyExisted);
    }
  }
  return id;
}

bool FaceDetectorServer::Save(std::string path) {
  bool is_success = false;
  is_success = face_engine_->Save(path.c_str());

  if (is_success) {
    spdlog::info("Saved seetaface database: {}", path);
  } else {
    spdlog::error("Failed to save seetaface database: {}", path);
  }

  return is_success;
}

bool FaceDetectorServer::Load(std::string path) {
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
  if (need_register_) {
    register_queue_.push(frame);
    need_register_ = false;
  }
  return;
}
