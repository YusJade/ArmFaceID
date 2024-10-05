#pragma once

#include <qobjectdefs.h>

#include <QImage>
#include <QString>
#include <string>
#include <vector>

#include <fmt/core.h>
#include <opencv2/opencv.hpp>
// #include <spdlog/fmt/bundled/core.h>
#include <spdlog/spdlog.h>

auto format_as(QString s) { return s.toStdString(); }

template <>
struct fmt::formatter<QString> : formatter<string_view> {
  auto format(const QString& qstr,
              format_context& ctx) const -> format_context::iterator;
};

#define ASSERET_WITH_LOG(msg, expr) \
  if (!expr) {                      \
    spdlog::error(msg);             \
    assert(expr);                   \
  }

namespace arm_face_id {
namespace utils {

// 将cv::Mat转换为QImage
QImage mat_to_qimage(const cv::Mat& mat);

cv::Mat qimage_to_mat(const QImage& qimage);

void mat_to_bytes(cv::Mat& mat, std::string ext, std::vector<uchar>& bytes);

void bytes_to_mat(std::string bytes_str, cv::Mat& res);
}  // namespace utils

}  // namespace arm_face_id
