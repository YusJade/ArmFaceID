#include "face_processor.h"

#include <QtConcurrent>
#include <memory>
#include <thread>
#include <vector>

#include <absl/log/absl_log.h>
#include <absl/strings/str_format.h>
#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>

#include "interface.h"

#include "face_network.pb.h"

using namespace arm_face_id;

void FaceProcessor::Run() {
  std::vector<cv::Rect> faces;
  int none_face_counter = 0;
  bool enable_rpc = true;
  while (!is_pause_) {
    if (frame_queue_.empty()) continue;
    auto frame = frame_queue_.front();
    frame_queue_.pop();

    faces.clear();
    classifier_.detectMultiScale(frame, faces);
    if (!faces.empty()) {
      none_face_counter = 0;
      if (enable_rpc) {
        // 向 RPC 服务端发送识别请求
        auto res = rpc_client_ptr_->RecognizeFace(frame);
        spdlog::info("接收到 RPC 服务器的识别结果：{}", res.id());
        for (auto iter : interface::FaceDetector<RecognizeResult>::observers_) {
          iter->OnFaceRecognized(frame, faces.front(), &res);
        }
        // 等待下次人脸进入检测范围
        enable_rpc = false;
      }
    } else {
      ++none_face_counter;
    }
    spdlog::info("none_face_counter:{}", none_face_counter);
    // 无人脸帧计数器达到阈值，确认上次人脸已经离开
    if (none_face_counter == threshold) {
      spdlog::info("人脸已离开检测范围~");
      enable_rpc = true;
    }
    // std::this_thread::sleep_for(std::chrono::seconds(2));
  }
}

void arm_face_id::FaceProcessor::Start() {
  work_thread_ = std::thread(&FaceProcessor::Run, this);
  is_pause_ = false;
  work_thread_.detach();
}

void arm_face_id::FaceProcessor::SetListener(
    std::shared_ptr<FaceProcessorListener>&& listener) {
  // listener_ptr_ = listener;
}

arm_face_id::FaceProcessor::FaceProcessor(const FaceProcessorSetting& setting) {

}

arm_face_id::FaceProcessor::FaceProcessor(
    std::shared_ptr<RpcClient> rpc_client_ptr, std::string model_path)
    : rpc_client_ptr_(rpc_client_ptr) {
  classifier_.load(model_path);
}

void arm_face_id::FaceProcessor::OnFrameCaptured(cv::Mat frame) {
  frame_queue_.push(frame);
}
