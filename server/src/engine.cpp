
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

#include <fmt/core.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <seeta/CFaceInfo.h>
#include <spdlog/spdlog.h>

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
  spdlog::info("已初始化人脸检测识别模块 ~");
  // spdlog::info(cv::getBuildInformation());
}

void FaceEngine::InitializeFeatures() {
  data::DBConnection db_conn;
  for (auto& usr : db_conn.SelectAllUser()) {
    // usr.face_img.save("loaded_face.jpg");
    cv::Mat cv_img = utils::qimage_to_mat(usr.face_img);
    // cv::imwrite("converted_face.jpg", cv_img);
    SeetaImageData simg{cv_img.cols, cv_img.rows, cv_img.channels(),
                        cv_img.data};
    std::vector<SeetaFaceInfo> face_infos = DetectFaces(simg);
    if (face_infos.empty()) {
      cv::imshow("load", cv_img);
      cv::waitKey();
      spdlog::error("用户 {} 的注册脸图失效！", usr.id);
      continue;
    }
    std::vector<float> feature = ExtractFaceFeature(simg, face_infos.front());
    users_.push_back(UserWithFeature{usr, feature});
  }
}

data::User FaceEngine::RecognizeFaceFromDb(const SeetaImageData& simg) {
  if (simg.channels != 3) {
    return data::User{-3};
  }
  data::User res{-1};
  auto faces = DetectFaces(simg);
  if (faces.empty()) {
    return res;
  }
  CompareFeaturesInDB(simg, faces.front(), &res);
  return res;
}

int64_t FaceEngine::RegisterFace(const SeetaImageData& simg,
                                 const SeetaFaceInfo& face_info,
                                 const data::User& user) {
  // 检查图片是否有效
  if (simg.channels != 3) {
    return -3;
  }

  // 检查数据库内是否已存在该人脸
  if (CompareFeaturesInDB(simg, face_info)) {
    return -2;
  }
  
  // 向数据库插入人脸信息
  data::DBConnection db_conn;
  int64_t id = db_conn.InsertUser(user);
  if (id == -1) {
    return -1;
  }
  // 更新内存中的人脸特征
  users_.push_back(UserWithFeature{user, ExtractFaceFeature(simg, face_info)});

  return id;
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

  data::DBConnection db_conn;
  for (auto& usr : users_) {
    {
      std::lock_guard<std::mutex> guard(mutex_);
      similarity = face_recognizer_->CalculateSimilarity(usr.feature.data(),
                                                         face_feature.data());
    }

    if (similarity > top_similarity) {
      top_similarity = similarity;
      matched_user = usr.user;
    }
  }
  if (top_similarity >= 0.8) {
    if (user) *user = matched_user;
    spdlog::info("检索到数据库内有相似脸部: id={}, similarity={}",
                 matched_user.id, top_similarity);
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
  spdlog::info("提取脸部特征点: {} 个", face_key_points.size());
  return std::move(face_key_points);
}

std::vector<SeetaFaceInfo> FaceEngine::DetectFaces(const SeetaImageData& simg) {
  SeetaFaceInfoArray faces = face_detector_->detect(simg);
  spdlog::info("检测到人脸: {} 张", faces.size);

  std::vector<SeetaFaceInfo> faces_vector;
  for (int i = 0; i < faces.size; ++i) {
    faces_vector.push_back(faces.data[i]);
    if (i >= faces.size) continue;
    spdlog::info("脸部 {} 得分: {}", i, faces.data[i].score);
  }

  return std::move(faces_vector);
}

float FaceEngine::CalculateFaceSimilarity(const std::vector<float>& feature1,
                                          const std::vector<float>& feature2) {
  float similarity = .0;
  similarity =
      face_recognizer_->CalculateSimilarity(feature1.data(), feature2.data());
  spdlog::info("两张脸部的相似度: {}", similarity);
  return similarity;
}
