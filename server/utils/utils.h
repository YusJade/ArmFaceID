#ifndef UTILS_H
#define UTILS_H

#include <vector>

#include <opencv2/opencv.hpp>

namespace utils {

void EncodeMat(cv::Mat& mat, std::string ext, std::vector<uchar>& bytes);

void DecodeMat(std::string bytes_str, cv::Mat& res);
}  // namespace utils

#endif  // UTILS_H
