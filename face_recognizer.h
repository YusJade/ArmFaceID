#ifndef FACE_RECOGNIZER_H
#define FACE_RECOGNIZER_H

#include <functional>
#include <thread>
#include <opencv4/opencv2/opencv.hpp>
// #include <memory>

class FaceRecognizer {
 public:
  FaceRecognizer();
  FaceRecognizer(std::function<void()>, std::function<void()>);
  void Start();

 private:
  std::function<void()> onCaptureHook;
  std::function<void()> onDetectHook;
  std::unique_ptr<std::thread> workThread;
  cv::VideoCapture videoCapture;
};

#endif  // FACE_RECOGNIZER_H
