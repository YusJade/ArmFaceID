#ifndef FACE_RECOGNIZER_H
#define FACE_RECOGNIZER_H

#ifdef _WIN32
    #include <opencv2/opencv.hpp>
#elif __linux__
    #include <opencv4/opencv2/opencv.hpp>
#endif
#include <functional>
#include <thread>

typedef std::function<void(cv::Mat image)> OnCaptureHook;
typedef std::function<void()> OnDetectHook;

class FaceRecognizer {
 public:
  FaceRecognizer();
  FaceRecognizer(OnCaptureHook, OnDetectHook);
  void Start();
  void DetectFace(cv::Mat image, std::vector<cv::Rect> &faces);

 private:
  cv::CascadeClassifier faceClassifier;
  OnCaptureHook onCaptureHook;
  OnDetectHook onDetectHook;
  std::unique_ptr<std::thread> workThread;
  cv::VideoCapture videoCapture;
};

#endif  // FACE_RECOGNIZER_H
