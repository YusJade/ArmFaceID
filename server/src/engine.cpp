
#include "engine.h"

#include <qdatetime.h>
#include <qglobal.h>

#include <algorithm>
#include <cstdint>
#include <ctime>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include <absl/time/clock.h>
#include <absl/time/time.h>
#include <fmt/core.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <seeta/CFaceInfo.h>
#include <spdlog/spdlog.h>

#include "database/schema.h"
#include "face_database.h"
#include "seeta/Common/CStruct.h"
#include "seeta/FaceDetector.h"
#include "seeta/FaceLandmarker.h"
#include "seeta/FaceRecognizer.h"
#include "utils/base.h"

using namespace arm_face_id;

FaceEngine::FaceEngine(const Settings& settings) {
  face_detector_ = std::make_unique<seeta::FaceDetector>(settings.fd_setting);
  face_recognizer_ =
      std::make_unique<seeta::FaceRecognizer>(settings.fr_setting);
  face_landmarker_ =
      std::make_unique<seeta::FaceLandmarker>(settings.pd_setting);
  std::cout << R"(
================================================================================

 █████╗ ██████╗ ███╗   ███╗      ███████╗ █████╗  ██████╗███████╗    ██╗██████╗ 
██╔══██╗██╔══██╗████╗ ████║      ██╔════╝██╔══██╗██╔════╝██╔════╝    ██║██╔══██╗
███████║██████╔╝██╔████╔██║█████╗█████╗  ███████║██║     █████╗█████╗██║██║  ██║
██╔══██║██╔══██╗██║╚██╔╝██║╚════╝██╔══╝  ██╔══██║██║     ██╔══╝╚════╝██║██║  ██║
██║  ██║██║  ██║██║ ╚═╝ ██║      ██║     ██║  ██║╚██████╗███████╗    ██║██████╔╝
╚═╝  ╚═╝╚═╝  ╚═╝╚═╝     ╚═╝      ╚═╝     ╚═╝  ╚═╝ ╚═════╝╚══════╝    ╚═╝╚═════╝ 
                                                                                
================================================================================
)" << std::endl;
  SPDLOG_INFO("Initialized FaceEngine :>");
  // spdlog::info(cv::getBuildInformation());
}

void FaceEngine::LoadFeaturesToMem(Schema& schema) {
  // data::DBConnection db_conn;
  for (auto& usr : schema.ListAllUsers()) {
    // usr.face_img.save(QString::fromStdString(
    //     fmt::format("./faces_loaded_{}.jpg",
    //     absl::FormatTime(absl::Now()))));
    cv::Mat cv_img = utils::qimage_to_mat(usr.face_img);
    // cv::imwrite("converted_face.jpg", cv_img);
    SeetaImageData simg{cv_img.cols, cv_img.rows, cv_img.channels(),
                        cv_img.data};
    std::vector<SeetaFaceInfo> face_infos = DetectFaces(simg);
    if (face_infos.empty()) {
      cv::imshow("load", cv_img);
      cv::waitKey();
      SPDLOG_WARN(
          "Failed to extract features from user`s face image (id={}), this "
          "user is diable!",
          usr.user_id);
      continue;
    }
    std::vector<float> feature = ExtractFaceFeature(simg, face_infos.front());
    face_features_cach_.push_back(FaceFeature{usr.user_id, feature});
  }
}

bool FaceEngine::CheckIfFaceExist(const SeetaImageData& simg,
                                  const SeetaFaceInfo& face, int64_t* id,
                                  float threshold) {
  auto checked_feature = ExtractFaceFeature(simg, face);
  float top_similarity = 0.0;
  int64_t top_face_id = -1;

  for (auto face_feature : face_features_cach_) {
    std::lock_guard<std::mutex> guard(mutex_);
    float cur_similarity = face_recognizer_->CalculateSimilarity(
        face_feature.feature.data(), checked_feature.data());
    if (cur_similarity > top_similarity) {
      top_similarity = cur_similarity;
      top_face_id = face_feature.id;
    }
  }

  SPDLOG_INFO("Top similar face id: {} ({})", top_face_id, top_similarity);

  if (top_similarity >= threshold) {
    if (id) *id = top_face_id;
    return true;
  }
  return false;
}

FaceEngine::QualityAnalyzeResult FaceEngine::AnalyzeQuality(
    const SeetaImageData& simg, const SeetaFaceInfo& face) {
  int feature_size = face_recognizer_->GetExtractFeatureSize();
  SeetaPointF points[feature_size];
  face_landmarker_->mark(simg, face.pos, points);

  auto resolution =
      resolution_assessor_.check(simg, face.pos, points, feature_size);
  auto clarity = clarity_assessor_.check(simg, face.pos, points, feature_size);
  auto integrity =
      integrity_assessor_.check(simg, face.pos, points, feature_size);
  auto pose = pose_assessor_.check(simg, face.pos, points, feature_size);

  return QualityAnalyzeResult{resolution.level, clarity.level, integrity.level,
                              pose.level};
}

bool FaceEngine::AddNewFeatureToMem(const SeetaImageData& simg,
                                    const SeetaFaceInfo& face,
                                    int64_t face_id) {
  auto features = ExtractFaceFeature(simg, face);
  face_features_cach_.push_back(FaceFeature{face_id, features});
  return true;
}

int64_t FaceEngine::RecognizeFaceFromMem(const SeetaImageData& simg,
                                         float threshold) {
  if (simg.channels != 3) {
    return -3;
  }
  auto faces = DetectFaces(simg);
  if (faces.empty()) {
    return -1;
  }
  auto face_feature = ExtractFaceFeature(simg, faces.front());
  float similarity = 0.0;
  float top_similarity = 0.0;
  int user_id = -1;
  for (auto& cur : face_features_cach_) {
    {
      std::lock_guard<std::mutex> guard(mutex_);
      similarity = face_recognizer_->CalculateSimilarity(cur.feature.data(),
                                                         face_feature.data());
      SPDLOG_INFO("{} Similarity with face(id={})", similarity, cur.id);
    }

    if (similarity > top_similarity) {
      user_id = cur.id;
      top_similarity = similarity;
    }
  }

  if (top_similarity < threshold) {
    return -1;
  }
  return user_id;
}

int64_t FaceEngine::RegisterFace(const SeetaImageData& simg,
                                 const SeetaFaceInfo& face_info,
                                 const data::User& user) {
  // TODO 细分错误类型
  // 检查图片是否有效
  if (simg.channels != 3) {
    return -3;
  }

  // 检查数据库内是否已存在该人脸
  if (CompareFeaturesInDB(simg, face_info)) {
    return -2;
  }

  // 向数据库插入人脸信息
  // data::DBConnection db_conn;
  // int64_t id = db_conn.InsertUser(user);
  // if (id == -1) {
  //   return -1;
  // }
  // // 更新内存中的人脸特征
  // face_features_cach_.push_back(
  //     FaceFeature{user.id, ExtractFaceFeature(simg, face_info)});

  return -1;
}

// 比对是否符合注册条件
bool FaceEngine::CompareFeaturesInDB(const SeetaImageData& simg,
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

  // data::DBConnection db_conn;
  for (auto& usr : face_features_cach_) {
    {
      std::lock_guard<std::mutex> guard(mutex_);
      similarity = face_recognizer_->CalculateSimilarity(usr.feature.data(),
                                                         face_feature.data());
    }

    if (similarity > top_similarity) {
      top_similarity = similarity;
      // matched_user = usr.user;
    }
  }
  if (top_similarity >= 0.83) {
    if (user) *user = matched_user;
    SPDLOG_INFO("id={}, similarity={}", matched_user.id, top_similarity);
    return true;
  }

  return false;
}

std::vector<float> FaceEngine::ExtractFaceFeature(const SeetaImageData& simg,
                                                  const SeetaFaceInfo& face) {
  std::vector<float> features(face_recognizer_->GetExtractFeatureSize());
  std::vector<SeetaPointF> face_key_points =
      face_landmarker_->mark(simg, face.pos);
  face_recognizer_->Extract(simg, face_key_points.data(), features.data());

  return std::move(features);
}

// std::vector<float> FaceDetectorServer::ExtractFaceFeature(
//     const SeetaImageData& simg,
//     const std::vector<SeetaPointF>& face_key_points) {
//   std::vector<float> features(face_recognizer_->GetExtractFeatureSize());
//   face_recognizer_->Extract(simg, face_key_points.data(), features.data());

//   return std::move(features);
// }

std::vector<SeetaPointF> FaceEngine::MarkFaceKeyPoints(
    const SeetaImageData& simg, const SeetaFaceInfo& face) {
  std::vector<SeetaPointF> face_key_points =
      face_landmarker_->mark(simg, face.pos);
  return std::move(face_key_points);
}

std::vector<SeetaFaceInfo> FaceEngine::DetectFaces(const SeetaImageData& simg) {
  SeetaFaceInfoArray faces = face_detector_->detect(simg);
  SPDLOG_INFO("Detectd {} faces on image.", faces.size);

  std::vector<SeetaFaceInfo> faces_vector;
  for (int i = 0; i < faces.size; ++i) {
    faces_vector.push_back(faces.data[i]);
    if (i >= faces.size) continue;
    SPDLOG_INFO("Face<{}> got score {}.", i, faces.data[i].score);
  }

  return std::move(faces_vector);
}

float FaceEngine::CalculateFaceSimilarity(const std::vector<float>& feature1,
                                          const std::vector<float>& feature2) {
  float similarity = .0;
  similarity =
      face_recognizer_->CalculateSimilarity(feature1.data(), feature2.data());
  SPDLOG_INFO("Similarity of the two faces: {}.", similarity);
  return similarity;
}
