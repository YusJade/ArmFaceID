#include "utils/base.h"

#include <fmt/core.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

using namespace arm_face_id;
// #include <spdlog/fmt/bundled/format.h>

auto fmt::formatter<QString>::format(const QString& qstr, format_context& ctx)
    const -> format_context::iterator {
  std::string str = qstr.toStdString();
  // ctx.out() = std::copy(str.begin(), str.end(), ctx.out());
  return formatter<string_view>::format(str, ctx);
}

QImage arm_face_id::utils::mat_to_qimage(const cv::Mat& mat) {
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
      cv::Mat mat_tmp;
      cv::cvtColor(mat, mat_tmp, cv::COLOR_BGR2RGB);
      QImage image(mat_tmp.data, mat_tmp.cols, mat_tmp.rows,
                   static_cast<int>(mat_tmp.step), QImage::Format_RGB888);
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

// 转换出Mat为四通道，无法被Seetaface识别
cv::Mat arm_face_id::utils::qimage_to_mat(const QImage& qimage) {
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

void arm_face_id::utils::mat_to_bytes(cv::Mat& mat, std::string ext,
                                      std::vector<uchar>& bytes) {
  std::vector<uchar> image_encoded;
  cv::imencode(ext, mat, image_encoded);
  bytes = std::vector<uchar>(image_encoded.begin(), image_encoded.end());
}

void arm_face_id::utils::bytes_to_mat(std::string bytes_str, cv::Mat& res) {
  std::vector<uchar> bytes(bytes_str.begin(), bytes_str.end());
  cv::Mat decode_img = cv::imdecode(bytes, cv::IMREAD_COLOR);
  if (decode_img.empty()) {
    std::cerr << "fail to decode img!\n";
    return;
  }
  res = decode_img;
  return;
}

QImage utils::generate_hash_avatar(const QString& username) {
  // 创建一个 625x625 的图像
  QImage image(625, 625, QImage::Format_RGB32);
  image.fill(Qt::white);

  // 使用 QCryptographicHash 生成哈希值
  QByteArray hash =
      QCryptographicHash::hash(username.toUtf8(), QCryptographicHash::Md5);

  // 将哈希值转换为颜色值
  QColor color;
  color.setRgb(hash[0], hash[1], hash[2]);

  // 使用 QPainter 绘制 5x5 的块
  QPainter painter(&image);
  painter.setBrush(color);
  painter.setPen(Qt::NoPen);

  for (int row = 0; row < 5; ++row) {
    for (int col = 0; col < 5; ++col) {
      // 计算块的位置和大小
      int x = col * 125;
      int y = row * 125;
      painter.drawRect(x, y, 125, 125);
    }
  }

  return image;
}