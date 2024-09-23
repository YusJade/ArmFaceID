#ifndef FACE_PROCESSOR_H
#define FACE_PROCESSOR_H

#include <memory>
#include <thread>

#include <opencv2/opencv.hpp>

#include "camera.h"
#include "face_processor_listener.h"
#include "rpc_client.h"

namespace arm_face_id {

/**
 * @brief FaceProcessor 的相关设置
 *
 */
class FaceProcessorSetting {
 public:
  // FaceProcessorSetting(int capture_device_id, std::string
  // capture_network_url,
  //                      std::string classifier_model_path,
  //                      std::string rpc_server_addr)
  //     : caputure_device_id_(capture_device_id),
  //       capture_network_url_(capture_network_url),
  //       classifier_model_path_(classifier_model_path),
  //       rpc_server_addr_(rpc_server_addr) {}

 private:
  // int caputure_device_id_ = 1;
  // std::string capture_network_url_;
  // std::string classifier_model_path_;
  // std::string rpc_server_addr_;

  // friend class FaceProcessor;
};

/**
 * @brief FaceProcessor 主要实现人脸检测、识别，并提供相应回调的注册。
 *
 */
class FaceProcessor : public ICamera {
 public:
  FaceProcessor(std::shared_ptr<RpcClient> rpc_client_ptr,
                std::string model_path);
  FaceProcessor(const FaceProcessorSetting& setting);
  FaceProcessor() = delete;
  ~FaceProcessor() = default;

  void OnFrameCaptured(cv::Mat frame) override;

  inline void Continue() { is_pause_ = false; }
  inline void Stop() { is_pause_ = true; }
  void Start();
  void SetListener(std::shared_ptr<FaceProcessorListener>&& listener);

 private:
  bool is_pause_ = false;
  ::std::shared_ptr<FaceProcessorListener> listener_ptr_;
  ::std::unique_ptr<std::thread> work_thread_;
  ::cv::CascadeClassifier classifier_;
  bool send_rpc = true;
  std::thread rpc_cnter_;

  bool is_last_frame_contains_face = false;
  bool is_cur_frame_contains_face = false;
  // ::cv::VideoCapture video_capture_;
  std::shared_ptr<RpcClient> rpc_client_ptr_;

  int process_cnter_ = 0;

  static int max_process_cnter_;
};

}  // namespace arm_face_id

#endif  // FACE_PROCESSOR_H
