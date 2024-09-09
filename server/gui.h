#pragma once

#include <qwidget.h>
#include <QApplication>
#include <cstdint>
#include <memory>
#include "face_engine.h"
#include <QWidget>
#include <opencv2/core/mat.hpp>

class GUI {
public:
  explicit GUI(std::shared_ptr<seeta::FaceEngine> engine_ptr);

  void Init();
  QWidget* Get();
private:
  std::shared_ptr<seeta::FaceEngine> engine_ptr_;
  cv::Mat captured_face_mat_;

  int64_t RegisterFace();
};
