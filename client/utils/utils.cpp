#include "utils.h"

QImage utils::matToQImage(const cv::Mat& mat) {
  // 检查cv::Mat是否为空
  if (mat.empty()) {
    return QImage();
  }

  switch (mat.type()) {
    // 8-bit, 1 channel
    case CV_8UC1: {
      QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Grayscale8);
      return image.copy(); // 创建QImage副本
    }
    // 8-bit, 3 channels
    case CV_8UC3: {
      QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_BGR888);
      return image.copy(); // 创建QImage副本
    }
    // 8-bit, 4 channels
    case CV_8UC4: {
      QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_RGBA8888);
      return image.copy(); // 创建QImage副本
    }
    default: {
      //未知格式
      return QImage();
    }
  }
}
