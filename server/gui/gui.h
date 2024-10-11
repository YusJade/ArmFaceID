#pragma once

#include <QApplication>
#include <QStackedWidget>
#include <QWidget>
#include <cstdint>
#include <functional>
#include <memory>

#include <absl/strings/internal/str_format/output.h>
#include <opencv2/core/mat.hpp>

#include "engine.h"
// #include "face_engine.h"

namespace arm_face_id {

class GUI {
 public:
  explicit GUI(std::shared_ptr<FaceDetectorServer> engine_ptr);

  void Init();
  QWidget* Get();

 private:
  std::shared_ptr<FaceDetectorServer> engine_ptr_;
  cv::Mat captured_face_mat_;
  QWidget* main_widget_ = nullptr;
  QStackedWidget* stacked_widget_ = nullptr;
  QWidget* rpc_widget_ = nullptr;
  QWidget* register_widget_ = nullptr;

  std::function<void(cv::Mat)> on_frame_captured_callback_;

  int64_t RegisterFace();
  QWidget* InitRpcWidget();
  QWidget* InitRegisterWidget();
};
}  // namespace arm_face_id
