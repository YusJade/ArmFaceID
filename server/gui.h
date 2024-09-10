#pragma once

#include <qstackedwidget.h>
#include <qwidget.h>

#include <QApplication>
#include <QWidget>
#include <cstdint>
#include <memory>

#include <opencv2/core/mat.hpp>

#include "engine.h"
#include "face_engine.h"

namespace arm_face_id {

class GUI : public ICamera {
 public:
  explicit GUI(std::shared_ptr<Engine> engine_ptr);

  void Init();
  QWidget* Get();

  void OnFrameCaptured(cv::Mat frame) override;

 private:
  std::shared_ptr<Engine> engine_ptr_;
  cv::Mat captured_face_mat_;
  QWidget* main_widget_ = nullptr;
  QStackedWidget* stacked_widget_ = nullptr;
  QWidget* rpc_widget_ = nullptr;
  QWidget* register_widget_ = nullptr;

  int64_t RegisterFace();
  QWidget* InitRpcWidget();
  QWidget* InitRegisterWidget();
};
}  // namespace arm_face_id
