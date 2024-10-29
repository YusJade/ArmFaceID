#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include <seeta/CFaceInfo.h>
#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>
#include <seeta/FaceRecognizer.h>

#include "face_database.h"
#include "seeta/Common/CStruct.h"

namespace arm_face_id {

namespace data {
struct User;
}

/**
 * @brief This Engine provide face detection,face recognintion,face register,and
 * persistence of related data, which will be injected into the objects depend
 * the engine.
 */
class FaceEngine {
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

 public:
  explicit FaceEngine(const Settings &);
  FaceEngine() = delete;
  FaceEngine(const FaceEngine &) = delete;

 public:
  void Start();
  // inline void Stop() { is_thread_running_ = false; }

  // 读取数据库脸图生成特征缓存。
  void InitializeFeatures();

  std::vector<float> ExtractFaceFeature(const SeetaImageData &,
                                        const SeetaFaceInfo &);

  std::vector<SeetaPointF> MarkFaceKeyPoints(const SeetaImageData &,
                                             const SeetaFaceInfo &);

  std::vector<SeetaFaceInfo> DetectFaces(const SeetaImageData &);

  float CalculateFaceSimilarity(const std::vector<float> &feature1,
                                const std::vector<float> &feature2);

  data::User RecognizeFaceFromDb(const SeetaImageData &);

  int64_t RegisterFace(const SeetaImageData &, const SeetaFaceInfo &,
                       const data::User &);

  /**
   * @brief
   * 与数据库缓存中的人脸特征值进行对比，若存在相似度大于阈值的特征，则返回
   * True， 该操作不会将数据库中的特征加载到缓存中，而是读取当前的缓存。
   * @param[in] simg 包含人脸的图像数据
   * @param[in] face 指定图像中的一个人脸信息
   * @param[out] user 若匹配到相似的人脸，则获得其信息
   * @return true 数据库中没有匹配的特征
   * @return false 在数据库中发现了匹配的特征
   */
  bool CompareFeaturesInDB(const SeetaImageData &simg,
                           const SeetaFaceInfo &face,
                           data::User *user = nullptr);

 private:
  std::mutex mutex_;
  std::unique_ptr<seeta::FaceLandmarker> face_landmarker_;
  std::unique_ptr<seeta::FaceDetector> face_detector_;
  std::unique_ptr<seeta::FaceRecognizer> face_recognizer_;

  struct UserWithFeature {
    data::User user;
    std::vector<float> feature;
  };
  std::vector<UserWithFeature> users_;
};
}  // namespace arm_face_id
