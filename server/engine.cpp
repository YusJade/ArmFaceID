
#include "engine.h"

#include <qdatetime.h>
#include <qglobal.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iterator>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include <fmt/core.h>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/flann/dist.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <seeta/CFaceInfo.h>
#include <spdlog/spdlog.h>

#include "face_database.h"
// #include "face_engine.h"
#include "function.h"
#include "interface.h"
#include "seeta/Common/CStruct.h"
#include "seeta/FaceDetector.h"
#include "seeta/FaceLandmarker.h"
#include "seeta/FaceRecognizer.h"

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

FaceDetectorServer::FaceDetectorServer(const Settings& settings) {
  face_detector_ = std::make_unique<seeta::FaceDetector>(settings.fd_setting);
  face_recognizer_ =
      std::make_unique<seeta::FaceRecognizer>(settings.fr_setting);
  face_landmarker_ =
      std::make_unique<seeta::FaceLandmarker>(settings.pd_setting);
  spdlog::info("已初始化人脸检测识别模块 ~");
  // spdlog::info(cv::getBuildInformation());
}

// FaceDetectorServer::FaceDetectorServer(const Settings& config)
//     : face_engine_(std::make_unique<seeta::FaceEngine>(
//           config.fd_setting, config.pd_setting, config.fr_setting)) {
//   ASSERET_WITH_LOG("无法加载级联分类器！",
//                    classifier_.load(config.classifier_path));
//   spdlog::info("已初始化人脸检测识别模块 ~");
//   // spdlog::info(cv::getBuildInformation());
// }

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
      // this->RegisterFeature(cur_frame);
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
  // classifier_.detectMultiScale(frame, faces);
}

data::User FaceDetectorServer::RecognizeFaceFromDb(const cv::Mat& img) {
  data::User matched_user;
  matched_user.id = -1;
  SeetaImageData simg{img.cols, img.rows, img.channels(), img.data};
  vector<SeetaFaceInfo> faces = DetectFaces(simg);
  if (faces.size() == 0) {
    return std::move(matched_user);
  }
  SeetaFaceInfo top_face = faces.front();
  // vector<SeetaPointF> face_key_point = MarkFaceKeyPoints(simg, top_face);
  vector<float> face_feature = ExtractFaceFeature(simg, top_face);

  float top_similarity = 0.0;
  float similarity = 0.0;
  // cv::Mat decoded_mat;
  auto& db = data::FaceDataBase::GetInstance();
  for (auto& user_in_db : db.Users()) {
    {
      std::lock_guard<std::mutex> guard(mutex_);
      similarity = CalculateFaceSimilarity(user_in_db.face_feature.data(),
                                           face_feature.data());
    }

    if (similarity > top_similarity) {
      top_similarity = similarity;
      matched_user = user_in_db;
    }
  }

  if (top_similarity < 0.9) {
    data::User user_not_matched;
    user_not_matched.id = -1;
    return std::move(user_not_matched);
    // TODO: 告知观察者
  } else {
    spdlog::info("识别到人脸: id={}, nick_name={}, similarity={} :>",
                 matched_user.id, matched_user.nick_name, similarity);
    for (auto iter : observers_) {
      iter->OnFaceRecognized(img, cv::Rect(), matched_user.id);
    }
  }

  return std::move(matched_user);
}

int64_t FaceDetectorServer::RegisterFace(const cv::Mat& frame,
                                         const data::User& user) {
  SeetaImageData simg{frame.cols, frame.rows, frame.channels(), frame.data};
  SeetaFaceInfoArray faces = face_detector_->detect(simg);
  if (faces.size == 0) {
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
  // auto id = this->RecognizeFaceFromDb(frame);
  data::User user_check = user;
  if (!CompareFeaturesInDB(simg, faces.data[0], &user_check)) {
    // cv::imshow("DB INS", frame);
    // cv::waitKey();
    // TODO: const cv::Mat& frame 源对象被修改了！！！
    data::FaceDataBase::GetInstance().AddUser(user_check);
    // 刷新内存中的用户信息
    data::FaceDataBase::GetInstance().LoadToCache();
    for (auto iter : observers_) {
      iter->OnFaceRegistered(frame, cv::Rect(), 999);
    }
  } else {
    spdlog::info("注册失败: 人脸的注册信息已经存在 :< (id={})", user_check.id);
    for (auto iter : observers_) {
      iter->OnFaceRegistered(
          frame, cv::Rect(),
          interface::FaceDetectorObserver<int64_t>::kFaceAlreadyExisted);
    }
  }
  return user_check.id;
}

bool FaceDetectorServer::Save(std::string path) {
  bool is_success = false;
  // is_success = face_engine_->Save(path.c_str());

  if (is_success) {
    spdlog::info("Saved seetaface database: {}", path);
  } else {
    spdlog::error("Failed to save seetaface database: {}", path);
  }

  return is_success;
}

bool FaceDetectorServer::Load(std::string path) {
  bool is_success = false;
  // is_success = face_engine_->Load(path.c_str());

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
    register_queue_.push(frame.clone());
    need_register_ = false;
  }
  return;
}

bool FaceDetectorServer::CompareFeaturesInDB(const SeetaImageData& simg,
                                             const SeetaFaceInfo& face,
                                             data::User* user) {
  float similarity = 0.0;
  float top_similarity = 0.0;
  data::User matched_user;
  // SeetaImageData seeta_img{img.cols, img.rows, img.channels(), img.data};
  QVector<float> face_feature;
  {
    std::lock_guard<std::mutex> guard(mutex_);
    auto face_feature_stl_vec = ExtractFaceFeature(simg, face);
    face_feature = QVector<float>(face_feature_stl_vec.begin(),
                                  face_feature_stl_vec.end());
  }
  if (face_feature.empty()) {
    return true;
  }

  data::FaceDataBase& db = data::FaceDataBase::GetInstance();
  for (auto& user_in_db : db.Users()) {
    {
      std::lock_guard<std::mutex> guard(mutex_);
      similarity = face_recognizer_->CalculateSimilarity(
          user_in_db.face_feature.data(), face_feature.data());
    }

    if (similarity > top_similarity) {
      top_similarity = similarity;
      matched_user = user_in_db;
    }
  }
  if (top_similarity >= 0.9) {
    if (user) {
      *user = matched_user;
    }
    spdlog::info("检索到数据库内有相似脸部: id={}, similarity={}",
                 matched_user.id, top_similarity);
    return true;
  }

  if (user) {
    user->face_feature = std::move(face_feature);
  }
  return false;
}

vector<float> FaceDetectorServer::ExtractFaceFeature(
    const SeetaImageData& simg, const SeetaFaceInfo& face) {
  vector<float> features(face_recognizer_->GetExtractFeatureSize());
  vector<SeetaPointF> face_key_points = face_landmarker_->mark(simg, face.pos);
  face_recognizer_->Extract(simg, face_key_points.data(), features.data());

  return std::move(features);
}

vector<float> FaceDetectorServer::ExtractFaceFeature(
    const SeetaImageData& simg, const vector<SeetaPointF>& face_key_points) {
  vector<float> features(face_recognizer_->GetExtractFeatureSize());
  face_recognizer_->Extract(simg, face_key_points.data(), features.data());

  return std::move(features);
}

vector<SeetaFaceInfo> FaceDetectorServer::DetectFaces(const cv::Mat& img) {
  SeetaImageData simg{img.cols, img.rows, img.channels(), img.data};
  SeetaFaceInfoArray faces = face_detector_->detect(simg);
  vector<SeetaFaceInfo> faces_vec(faces.size);
  for (int i = 0; i < faces.size; ++i) {
    faces_vec[i] = faces.data[i];
  }
  return std::move(faces_vec);
}

vector<SeetaPointF> FaceDetectorServer::MarkFaceKeyPoints(
    const SeetaImageData& simg, const SeetaFaceInfo& face) {
  vector<SeetaPointF> face_key_points = face_landmarker_->mark(simg, face.pos);
  spdlog::info("提取脸部特征点: {} 个", face_key_points.size());
  return std::move(face_key_points);
}

vector<SeetaFaceInfo> FaceDetectorServer::DetectFaces(
    const SeetaImageData& simg) {
  SeetaFaceInfoArray faces = face_detector_->detect(simg);
  spdlog::info("检测到人脸: {} 张", faces.size);

  vector<SeetaFaceInfo> faces_vector;
  for (int i = 0; i < faces.size; ++i) {
    faces_vector.push_back(faces.data[i]);
    if (i >= faces.size) continue;
    spdlog::info("脸部 {} 得分: {}", i, faces.data[i].score);
  }

  return std::move(faces_vector);
}

float FaceDetectorServer::CalculateFaceSimilarity(
    const vector<float>& feature1, const vector<float>& feature2) {
  float similarity = .0;
  similarity =
      face_recognizer_->CalculateSimilarity(feature1.data(), feature2.data());
  spdlog::info("两张脸部的相似度: {}", similarity);
  return similarity;
}

float FaceDetectorServer::CalculateFaceSimilarity(const float* feature1,
                                                  const float* feature2) {
  float similarity = .0;
  similarity = face_recognizer_->CalculateSimilarity(feature1, feature2);
  spdlog::info("两张脸部的相似度: {}", similarity);
  return similarity;
}