#include "utils/base.h"

#include <qbuffer.h>
#include <qimage.h>
#include <qnamespace.h>
#include <qstringview.h>

#include <fmt/core.h>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <spdlog/spdlog.h>

using namespace arm_face_id;
// #include <spdlog/fmt/bundled/format.h>

auto fmt::formatter<QString>::format(const QString& qstr,
                                     format_context& ctx) const
    -> format_context::iterator {
  std::string str = qstr.toStdString();
  // ctx.out() = std::copy(str.begin(), str.end(), ctx.out());
  return formatter<string_view>::format(str, ctx);
}

QImage utils::base64_to_qimage(const QString& base64_str,
                               const QString& format) {
  QByteArray byte_arr(base64_str.toUtf8());
  QBuffer buffer(&byte_arr);
  buffer.open(QIODevice::ReadOnly);
  QImage img;
  bool success = img.load(&buffer, format.toStdString().c_str());
  if (!success) {
    spdlog::warn("无法将 Base64 解码为图片！");
  }
  return img;
}

QImage utils::mat_to_qimage(const cv::Mat& mat) {
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
  QImage formatted = qimage.convertedTo(QImage::Format_RGB888);
  switch (formatted.format()) {
    case QImage::Format_RGB32: {
      mat = cv::Mat(formatted.height(), formatted.width(), CV_8UC4,
                    (void*)formatted.bits(), formatted.bytesPerLine());
      break;
    }
    case QImage::Format_RGB888: {
      mat = cv::Mat(formatted.height(), formatted.width(), CV_8UC3,
                    (void*)formatted.bits(), formatted.bytesPerLine());
      cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
      break;
    }
    case QImage::Format_Grayscale8: {
      mat = cv::Mat(formatted.height(), formatted.width(), CV_8UC1,
                    (void*)formatted.bits(), formatted.bytesPerLine());
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
  // 创建一个 250x250 的图像
  const int blockSize = 50;  // 每个块的大小
  const int gridSize = 5;    // 网格尺寸（5x5）
  const int imageSize = gridSize * blockSize;
  QImage image(imageSize, imageSize, QImage::Format_RGB32);
  image.fill(Qt::white);

  // 使用 QCryptographicHash 生成哈希值
  QByteArray hash =
      QCryptographicHash::hash(username.toUtf8(), QCryptographicHash::Md5);

  // 将哈希值的前三个字节作为颜色值
  QColor color;
  color.setRgb(static_cast<quint8>(hash[0]), static_cast<quint8>(hash[1]),
               static_cast<quint8>(hash[2]));

  QPainter painter(&image);
  painter.setBrush(color);
  painter.setPen(Qt::NoPen);

  // 遍历 5x5 网格，仅绘制对称的左侧
  for (int row = 0; row < gridSize; ++row) {
    for (int col = 0; col < gridSize / 2 + 1; ++col) {  // 左侧 + 中间列
      // 使用哈希值决定当前块是否填充颜色
      int hashIndex = row * gridSize + col;
      if (hashIndex < hash.size() &&
          static_cast<quint8>(hash[hashIndex]) % 2 == 0) {
        // 计算块的位置
        int x = col * blockSize;
        int y = row * blockSize;

        // 绘制对称的块
        painter.drawRect(x, y, blockSize, blockSize);  // 左侧块
        painter.drawRect((gridSize - 1 - col) * blockSize, y, blockSize,
                         blockSize);  // 对称块
      }
    }
  }

  return image;
}
