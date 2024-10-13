#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <seeta/CFaceInfo.h>
#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>
#include <seeta/FaceRecognizer.h>

#include "face_database.h"
// #include "face_engine.h"
#include "interface.h"
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
class FaceDetectorServer : public interface::FaceDetector<int64_t>,
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

  static shared_ptr<FaceDetectorServer> BuildAndReturn(
      const Settings &settings);
  static shared_ptr<FaceDetectorServer> GetInstance();

 private:
  explicit FaceDetectorServer(const Settings &);
  FaceDetectorServer() = delete;
  FaceDetectorServer(const FaceDetectorServer &) = delete;

 public:
  void Start();
  inline void Stop() { is_thread_running_ = false; }

  vector<float> ExtractFaceFeature(const SeetaImageData &,
                                   const SeetaFaceInfo &);
  vector<float> ExtractFaceFeature(const SeetaImageData &,
                                   const vector<SeetaPointF> &);

  vector<SeetaPointF> MarkFaceKeyPoints(const SeetaImageData &,
                                        const SeetaFaceInfo &);

  vector<SeetaFaceInfo> DetectFaces(const cv::Mat &);
  vector<SeetaFaceInfo> DetectFaces(const SeetaImageData &);

  float CalculateFaceSimilarity(const vector<float> &feature1,
                                const vector<float> &feature2);

  float CalculateFaceSimilarity(const float *feature1, const float *feature2);

  data::User RecognizeFaceFromDb(const cv::Mat &);

  int64_t RegisterFace(const cv::Mat &, const data::User &);

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
  [[deprecated]]
  void DetectFace(std::vector<cv::Rect> &, const cv::Mat &);

  bool Save(std::string path);
  bool Load(std::string path);

  inline void NeedRegisterFace(const data::User &user) {
    need_register_ = true;
    next_register_user_ = user;
  }
  inline void NeedRecognizeFace(bool toggled) { need_recognize_ = toggled; }

  virtual void OnCameraShutDown() override;
  virtual void OnFrameCaptured(cv::Mat frame) override;

 private:
  bool need_register_ = false;
  bool need_recognize_ = false;
  // std::unique_ptr<seeta::FaceEngine> face_engine_;
  std::unique_ptr<std::thread> worker_thread_;
  std::unique_ptr<std::thread> register_thread_;
  bool is_thread_running_ = false;
  // cv::CascadeClassifier classifier_;
  std::mutex mutex_;
  std::queue<cv::Mat> frame_queue_;
  std::queue<cv::Mat> register_queue_;
  data::User next_register_user_;
  static std::shared_ptr<FaceDetectorServer> _instance;

  std::unique_ptr<seeta::FaceLandmarker> face_landmarker_;
  std::unique_ptr<seeta::FaceDetector> face_detector_;
  std::unique_ptr<seeta::FaceRecognizer> face_recognizer_;

  vector<float> last_features_;
};
}  // namespace arm_face_id
