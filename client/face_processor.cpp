#include "face_processor.h"

#include <QtConcurrent>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>

#include <absl/log/absl_log.h>
#include <absl/strings/str_format.h>
#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>

#include "utils.h"

void arm_face_id::FaceProcessor::Start() {
  work_thread_ = std::thread([&] {
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
  });
  is_pause_ = false;
  work_thread_.detach();
  // rpc_cnter_.detach();
  // if (!listener_ptr_) {
  //   std::cerr << "fail to find listener, the service will not be started.";
  //   return;
  // }
  // work_thread_ = std::make_unique<std::thread>([&] {
  //   bool detect_once = false;
  //   while (true) {
  //     cv::Mat frame;
  //     video_capture_.read(frame);
  //     if (frame.empty()) {
  //       continue;
  //     }
  //     // opencv 实现人脸检测
  //     std::vector<cv::Rect> faces;
  //     cv::Mat frame_gray;

  //     cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
  //     cv::equalizeHist(frame_gray, frame_gray);

  //     // 若检测到人脸，则提交到服务端进行识别
  //     face_classifier_.detectMultiScale(frame_gray, faces);
  //     if (!faces.empty() && !detect_once) {
  //       detect_once = true;
  //       auto face_rect = faces.at(0);
  //       cv::rectangle(frame, face_rect, cv::Scalar(0, 255, 255));
  //       this->listener_ptr_->OnFaceDetected(frame, face_rect);
  //       // 通过 cv::Mat 的重载运算符截取出矩形部分
  //       cv::Mat face_mat(frame(face_rect));
  //       auto rpc_res = rpc_client_.RecognizeFace(face_mat);

  //       FaceProcessorListener::RecognitionResult recognition_result;
  //       recognition_result.id = rpc_res.id();
  //       recognition_result.name = rpc_res.name();
  //       listener_ptr_->OnFaceRecognized(recognition_result);
  //     } else if (faces.empty()) {
  //       detect_once = false;
  //     }
  //     listener_ptr_->OnImageCaptured(frame);
  //     std::this_thread::sleep_for(std::chrono::microseconds(100));
  //   }
  // });
  // work_thread_->detach();
}

void arm_face_id::FaceProcessor::SetListener(
    std::shared_ptr<FaceProcessorListener>&& listener) {
  // listener_ptr_ = listener;
}

arm_face_id::FaceProcessor::FaceProcessor(const FaceProcessorSetting& setting) {
  // : rpc_client_(grpc::CreateChannel(setting.rpc_server_addr_,
  //                                   grpc::InsecureChannelCredentials())) {
  // if (!face_classifier_.load(setting.classifier_model_path_)) {
  //   std::cerr << absl::StrFormat("failed to load cascade model at %s!\n",
  //                                setting.classifier_model_path_);
  // }
  // std::cerr << absl::StrFormat("loaded cascade model at %s. :>\n",
  //                              setting.classifier_model_path_);

  // // 若设置使用网络摄像头，则优先打开网络摄像头
  // bool can_access_network_camera = false;
  // if (!setting.capture_network_url_.empty()) {
  //   can_access_network_camera =
  //       video_capture_.open(setting.capture_network_url_);
  //   if (!can_access_network_camera) {
  //     std::cerr << absl::StrFormat(
  //         "failed to open network camera,maybe check your network?\n");
  //   } else {
  //     ABSL_LOG(INFO) << absl::StrFormat("open the camera.\n");
  //     return;
  //   }
  // }
  // if (!can_access_network_camera &&
  //     !video_capture_.open(setting.caputure_device_id_)) {
  //   std::cerr << absl::StrFormat(
  //       "failed to open the camera on your device,check your device`s "
  //       "state!\n");
  //   return;
  // }
  // ABSL_LOG(INFO) << absl::StrFormat("open the camera.\n");
}

arm_face_id::FaceProcessor::FaceProcessor(
    std::shared_ptr<RpcClient> rpc_client_ptr, std::string model_path)
    : rpc_client_ptr_(rpc_client_ptr) {
  classifier_.load(model_path);
}

void arm_face_id::FaceProcessor::OnFrameCaptured(cv::Mat frame) {
  frame_queue_.push(frame);

  // is_last_frame_contains_face = is_cur_frame_contains_face;
  // is_cur_frame_contains_face = !faces.empty();

  // if (!faces.empty() && send_rpc) {
  //   send_rpc = false;
  //   std::thread thread([=] {
  //     auto response = rpc_client_ptr_->RecognizeFace(frame);
  //     spdlog::info("Recieved response: id={}", response.id());
  //   });
  //   thread.detach();
  // }
}
