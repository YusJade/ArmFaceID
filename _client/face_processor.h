#ifndef FACE_PROCESSOR_H
#define FACE_PROCESSOR_H

#include <thread>

#include <opencv2/opencv.hpp>

#include "face_processor_listener.h"
#include "rpc_client.h"

namespace arm_face_id {

/**
 * @brief FaceProcessor 的相关设置
 *
 */
class FaceProcessorSetting {
 public:
  FaceProcessorSetting(int capture_device_id, std::string capture_network_url,
                       std::string classifier_model_path,
                       std::string rpc_server_addr)
      : caputure_device_id_(capture_device_id),
        capture_network_url_(capture_network_url),
        classifier_model_path_(classifier_model_path),
        rpc_server_addr_(rpc_server_addr) {}

 private:
  int caputure_device_id_ = 1;
  std::string capture_network_url_;
  std::string classifier_model_path_;
  std::string rpc_server_addr_;

  friend class FaceProcessor;
};

/**
 * @brief FaceProcessor 主要实现人脸检测、识别，并提供相应回调的注册。
 *
 */
class FaceProcessor {
 public:
  FaceProcessor(const FaceProcessorSetting& setting);
  FaceProcessor() = delete;
  ~FaceProcessor() = default;

  void Start();
  void RegisterListener(std::shared_ptr<FaceProcessorListener>&& listener);

 private:
  ::std::shared_ptr<FaceProcessorListener> listener_ptr_;
  ::std::unique_ptr<std::thread> work_thread_;
  ::cv::CascadeClassifier face_classifier_;
  ::cv::VideoCapture video_capture_;
  RpcClient rpc_client_;
};

}  // namespace arm_face_id

#endif  // FACE_PROCESSOR_H