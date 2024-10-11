
#include "engine.h"

#include <qdatetime.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include <fmt/core.h>
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
  detector_ = std::make_unique<seeta::FaceDetector>(settings.fd_setting);
  recognizer_ = std::make_unique<seeta::FaceRecognizer>(settings.fr_setting);
  landmarker_ = std::make_unique<seeta::FaceLandmarker>(settings.pd_setting);
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

int64_t FaceDetectorServer::RecognizeFace(const cv::Mat& frame) {
  std::lock_guard<std::mutex> lock_guard(mutex_);
  SeetaImageData img_date{frame.cols, frame.rows, frame.channels(), frame.data};
  float similarity = .0;
  int64_t id = -2;
  // id = face_engine_->Query(img_date, &similarity);
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
  // auto id = face_engine_->Query(img_date, &similarity);
  // if (id != -1 && similarity > 0.5) {
  //   spdlog::info("注册失败: 人脸的注册信息已经存在 :< (id={},
  //   similarity={})",
  //                id, similarity);

  //   for (auto iter : observers_) {
  //     iter->OnFaceRegistered(
  //         frame, cv::Rect(),
  //         interface::FaceDetectorObserver<int64_t>::kFaceAlreadyExisted);
  //   }

  //   return interface::FaceDetectorObserver<int64_t>::kFaceAlreadyExisted;
  // }
  // id = face_engine_->Register(img_date);
  // if (id == -1) {
  //   spdlog::info("注册失败: 检测不到人脸 :< \n");
  //   for (auto iter : observers_) {
  //     iter->OnFaceRegistered(
  //         frame, cv::Rect(),
  //         interface::FaceDetectorObserver<int64_t>::kFaceNotDetected);
  //   }
  // } else {
  //   spdlog::info("注册成功: id= {} :> \n", id);

  //   for (auto iter : observers_) {
  //     iter->OnFaceRegistered(frame, cv::Rect(), id);
  //   }
  // }
  // return id;
  return 0;
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
      // similarity = face_engine_->Compare(target_img, db_img);
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

int64_t FaceDetectorServer::RegisterFace(const cv::Mat& frame,
                                         const data::User& user) {
  SeetaImageData simg{frame.cols, frame.rows, frame.channels(), frame.data};
  SeetaFaceInfoArray faces = detector_->detect(simg);
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
  if (!IsAlreadyExistInDB(simg, faces.data[0], &user_check)) {
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
    register_queue_.push(frame);
    need_register_ = false;
  }
  return;
}

int64_t FaceDetectorServer::RegisterFeature(const cv::Mat& img) {
  vector<SeetaFaceInfo> faces;
  SeetaImageData seeta_img{img.cols, img.rows, img.channels(), img.data};
  // faces = face_engine_->DetectFaces(seeta_img);
  if (faces.empty()) {
    spdlog::warn("SeetaFace 检测不到人脸 :<");
    return -1;
  }
  SeetaFaceInfo top_face = faces.front();
  spdlog::info("top_face: pos=({},{}),{}*{}, score={}", top_face.pos.x,
               top_face.pos.y, top_face.pos.width, top_face.pos.height,
               top_face.score);
  // if (IsAlreadyExistInDB(img)) {

  // }
  // int feature_size = face_engine_->FR.GetExtractFeatureSize();
  // vector<float> features(feature_size);
  // std::vector<SeetaPointF> points(face_engine_->PD.number());

  auto start = std::chrono::high_resolution_clock::now();

  // face_engine_->PD.mark(seeta_img, top_face.pos, points.data());
  // face_engine_->FR.Extract(seeta_img, points.data(), features.data());

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;
  spdlog::info("特征提取耗时: {}", duration.count());

  float simlarity = 0.0;
  if (last_features_.size() > 0) {
    start = std::chrono::high_resolution_clock::now();
    // simlarity = face_engine_->FR.CalculateSimilarity(features.data(),
    //                                                  last_features_.data());
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    spdlog::info("特征对比耗时: {}", duration.count());
  }

  spdlog::info("{} 与上一张人脸的相似度 :B", simlarity);
  // last_features_ = features;
  // data::FaceDataBase::GetInstance().features_vec_.push_back(features);
  for (auto iter : observers_) {
    iter->OnFaceRegistered(
        img, cv::Rect(),
        interface::FaceDetectorObserver<int64_t>::kFaceAlreadyExisted);
  }
  // // 刷新内存中的用户信息
  // data::FaceDataBase::GetInstance().LoadToCache();
  // for (auto iter : observers_) {
  //   iter->OnFaceRegistered(frame, cv::Rect(), 999);
  // }

  return -1;
}

// TODO: 尝试使用 SeetaFace 的特征值对比
bool FaceDetectorServer::IsAlreadyExistInDB(const SeetaImageData& simg,
                                            const SeetaFaceInfo& face,
                                            data::User* user) {
  float similarity = 0.0;
  float top_similarity = 0.0;
  data::User matched_user;
  // SeetaImageData seeta_img{img.cols, img.rows, img.channels(), img.data};
  vector<float> face_feature;
  {
    std::lock_guard<std::mutex> guard(mutex_);
    face_feature = ExtractFaceFeature(simg, face);
  }
  if (face_feature.empty()) {
    return true;
  }

  data::FaceDataBase& db = data::FaceDataBase::GetInstance();
  for (auto& user_in_db : db.Users()) {
    {
      std::lock_guard<std::mutex> guard(mutex_);
      similarity = recognizer_->CalculateSimilarity(
          user_in_db.face_feature.data(), face_feature.data());
    }

    if (similarity > top_similarity) {
      top_similarity = similarity;
      matched_user = user_in_db;
    }
  }
  if (top_similarity >= 0.6) {
    if (user) {
      *user = matched_user;
    }
    return true;
  }

  if (user) {
    user->face_feature = std::move(face_feature);
  }
  return false;
}

vector<float> FaceDetectorServer::ExtractFaceFeature(
    const SeetaImageData& simg, const SeetaFaceInfo& face) {
  vector<float> features(recognizer_->GetExtractFeatureSize());
  vector<SeetaPointF> face_key_points = landmarker_->mark(simg, face.pos);
  recognizer_->Extract(simg, face_key_points.data(), features.data());

  return std::move(features);
}

vector<SeetaFaceInfo> FaceDetectorServer::DetectFace(const cv::Mat& img) {
  SeetaImageData simg{img.cols, img.rows, img.channels(), img.data};
  SeetaFaceInfoArray faces = detector_->detect(simg);
  vector<SeetaFaceInfo> faces_vec(faces.size);
  for (int i = 0; i < faces.size; ++i) {
    faces_vec[i] = faces.data[i];
  }
  return std::move(faces_vec);
}
