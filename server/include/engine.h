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
#include <seeta/QualityOfClarity.h>
#include <seeta/QualityOfIntegrity.h>
#include <seeta/QualityOfPose.h>
#include <seeta/QualityOfResolution.h>

#include "Common/Struct.h"
#include "QualityStructure.h"
#include "database/schema.h"
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
  };

  struct QualityAnalyzeResult {
    seeta::QualityLevel resolution;
    seeta::QualityLevel clarity;
    seeta::QualityLevel integrity;
    seeta::QualityLevel pose;
  };

 public:
  explicit FaceEngine(const Settings &);
  FaceEngine() = delete;
  FaceEngine(const FaceEngine &) = delete;

 public:
  void Start();
  // inline void Stop() { is_thread_running_ = false; }

  // 读取数据库脸图生成特征缓存。
  void LoadFeaturesToMem(Schema &schema);

  QualityAnalyzeResult AnalyzeQuality(const SeetaImageData &simg,
                                      const SeetaFaceInfo &face);

  bool AddNewFeatureToMem(const SeetaImageData &simg, const SeetaFaceInfo &face,
                          int64_t face_id);

  /**
   * @brief 从内存缓存中检查系统内是否存在相似的人脸
   *
   * @param simg
   * @param face
   * @param id
   * @return true
   * @return false
   */
  bool CheckIfFaceExist(const SeetaImageData &simg, const SeetaFaceInfo &face,
                        int64_t *id = nullptr, float threshold = 0.85);

  std::vector<float> ExtractFaceFeature(const SeetaImageData &,
                                        const SeetaFaceInfo &);

  std::vector<SeetaPointF> MarkFaceKeyPoints(const SeetaImageData &,
                                             const SeetaFaceInfo &);

  std::vector<SeetaFaceInfo> DetectFaces(const SeetaImageData &);

  float CalculateFaceSimilarity(const std::vector<float> &feature1,
                                const std::vector<float> &feature2);

  int64_t RecognizeFaceFromMem(const SeetaImageData &, float threshold = 0.85);

  /**
   * @brief 注册人脸身份
   *
   * @return int64_t -1 表示注册失败
   */
  [[deprecated("剥离业务逻辑")]] int64_t RegisterFace(const SeetaImageData &,
                                                      const SeetaFaceInfo &,
                                                      const data::User &);

  /**
   * @brief
   * 与内存中的人脸特征值进行对比，若存在相似度大于阈值的特征，则返回
   * True
   * @param[in] simg 包含人脸的图像数据
   * @param[in] face 指定图像中的一个人脸信息
   * @param[out] user 若匹配到相似的人脸，则获得其信息
   * @return true 数据库中没有匹配的特征
   * @return false 在数据库中发现了匹配的特征
   */
  [[deprecated]] bool CompareFeaturesInDB(const SeetaImageData &simg,
                                          const SeetaFaceInfo &face,
                                          data::User *user = nullptr);

 private:
  std::mutex mutex_;
  std::unique_ptr<seeta::FaceLandmarker> face_landmarker_;
  std::unique_ptr<seeta::FaceDetector> face_detector_;
  std::unique_ptr<seeta::FaceRecognizer> face_recognizer_;
  seeta::QualityOfResolution resolution_assessor_;  // 人脸分辨率评估器
  seeta::QualityOfClarity clarity_assessor_;        // 人脸清晰度评估器
  seeta::QualityOfIntegrity integrity_assessor_;    // 人脸完整度评估器
  seeta::QualityOfPose pose_assessor_;              // 人脸姿态评估器
  struct FaceFeature {
    int64_t id = -1;
    std::vector<float> feature;
  };
  std::vector<FaceFeature> face_features_cach_;
};
}  // namespace arm_face_id
