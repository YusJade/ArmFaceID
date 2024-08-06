#ifndef UTILS_H
#define UTILS_H

#include <QImage>
#include <opencv2/opencv.hpp>

namespace utils {

// 将cv::Mat转换为QImage
QImage matToQImage(const cv::Mat& mat);

}

#endif  // UTILS_H
