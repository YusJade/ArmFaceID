#include "utils.h"

QImage utils::MatToQImage(const cv::Mat& mat) {
  // 检查cv::Mat是否为空
  if (mat.empty()) {
    return QImage();
  }

  switch (mat.type()) {
    // 8-bit, 1 channel
    case CV_8UC1: {
      QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step),
                   QImage::Format_Grayscale8);
      return image.copy();  // 创建QImage副本
    }
    // 8-bit, 3 channels
    case CV_8UC3: {
      QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step),
                   QImage::Format_RGB888);
      return image.copy();  // 创建QImage副本
    }
    // 8-bit, 4 channels
    case CV_8UC4: {
      QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step),
                   QImage::Format_RGBA8888);
      return image.copy();  // 创建QImage副本
    }
    default: {
      // 未知格式
      return QImage();
    }
  }
}

cv::Mat utils::QImageToMat(const QImage& qimage) {
  cv::Mat mat;
  switch (qimage.format()) {
    case QImage::Format_RGB32: {
      mat = cv::Mat(qimage.height(), qimage.width(), CV_8UC4,
                    (void*)qimage.bits(), qimage.bytesPerLine());
      break;
    }
    case QImage::Format_RGB888: {
      mat = cv::Mat(qimage.height(), qimage.width(), CV_8UC3,
                    (void*)qimage.bits(), qimage.bytesPerLine());
      cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
      break;
    }
    case QImage::Format_Grayscale8: {
      mat = cv::Mat(qimage.height(), qimage.width(), CV_8UC1,
                    (void*)qimage.bits(), qimage.bytesPerLine());
      break;
    }
    default: {
      // Unsupported format
      mat = cv::Mat();
      break;
    }
  }
  return mat;
}

void utils::EncodeMat(cv::Mat& mat, std::string ext,
                      std::vector<uchar>& bytes) {
  std::vector<uchar> image_encoded;
  cv::imencode(ext, mat, image_encoded);
  bytes = std::vector<uchar>(image_encoded.begin(), image_encoded.end());
}

void utils::DecodeMat(std::string bytes_str, cv::Mat& res) {
  std::vector<uchar> bytes(bytes_str.begin(), bytes_str.end());
  cv::Mat decode_img = cv::imdecode(bytes, cv::IMREAD_COLOR);
  if (decode_img.empty()) {
    std::cerr << "fail to decode img!\n";
    return;
  }
  res = decode_img;
  return;
}
