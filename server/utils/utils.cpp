#include "utils.h"

void utils::encodeMat(cv::Mat& mat, std::string ext,
                      std::vector<uchar>& bytes) {
  std::vector<uchar> image_encoded;
  cv::imencode(ext, mat, image_encoded);
  bytes = std::vector<uchar>(image_encoded.begin(), image_encoded.end());
}

void utils::decodeMat(std::string bytes_str, cv::Mat& res) {
  std::vector<uchar> bytes(bytes_str.begin(), bytes_str.end());
  cv::Mat decode_img = cv::imdecode(bytes, cv::IMREAD_COLOR);
  if (decode_img.empty()) {
    std::cerr << "fail to decode img!\n";
    return;
  }
  res = decode_img;
  return;
}
