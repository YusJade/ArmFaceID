#include "face_processor.h"

#include <absl/log/absl_log.h>
#include <absl/strings/str_format.h>

void arm_face_id::FaceProcessor::Start() {
  if (!listener_ptr_) {
    std::cerr << "fail to find listener, the service will not be started.";
    return;
  }
  work_thread_ = std::make_unique<std::thread>([&] {
    while (true) {
      cv::Mat frame;
      video_capture_.read(frame);
      if (frame.empty()) {
        continue;
      }

      // opencv 实现人脸检测
      std::vector<cv::Rect> faces;
      cv::Mat frame_gray;

      cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
      cv::equalizeHist(frame_gray, frame_gray);

      face_classifier_.detectMultiScale(frame_gray, faces);
      if (!faces.empty()) {
        auto face_rect = faces.at(0);
        cv::rectangle(frame, face_rect, cv::Scalar(0, 255, 255));

        // 通过 cv::Mat 的重载运算符截取出矩形部分
        listener_ptr_->OnFaceDetected(frame(face_rect), face_rect);
      }
      listener_ptr_->OnImageCaptured(frame);
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
  });
  work_thread_->detach();
}

void arm_face_id::FaceProcessor::RegisterListener(
    std::shared_ptr<FaceProcessorListener>&& listener) {
  listener_ptr_ = listener;
}

arm_face_id::FaceProcessor::FaceProcessor(const FaceProcessorSetting& setting)
    : rpc_client_(grpc::CreateChannel(setting.rpc_server_addr_,
                                      grpc::InsecureChannelCredentials())) {
  if (!face_classifier_.load(setting.classifier_model_path_)) {
    std::cerr << absl::StrFormat("failed to load cascade model at %s!\n",
                                 setting.classifier_model_path_);
  }
  std::cerr << absl::StrFormat("loaded cascade model at %s. :>\n",
                               setting.classifier_model_path_);

  // 若设置使用网络摄像头，则优先打开网络摄像头
  bool can_access_network_camera = false;
  if (!setting.capture_network_url_.empty()) {
    can_access_network_camera =
        video_capture_.open(setting.capture_network_url_);
    if (!can_access_network_camera) {
      std::cerr << absl::StrFormat(
          "failed to open network camera,maybe check your network?\n");
    } else {
      ABSL_LOG(INFO) << absl::StrFormat("open the camera.\n");
      return;
    }
  }
  if (!can_access_network_camera &&
      !video_capture_.open(setting.caputure_device_id_)) {
    std::cerr << absl::StrFormat(
        "failed to open the camera on your device,check your device`s "
        "state!\n");
    return;
  }
  ABSL_LOG(INFO) << absl::StrFormat("open the camera.\n");
}
