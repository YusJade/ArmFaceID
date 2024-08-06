#include "face_recognizer.h"

#include <QString>
#include <QDebug>

FaceRecognizer::FaceRecognizer():
  onCaptureHook(), onDetectHook(), videoCapture(1) {

  if (!faceClassifier.load("D:\\opencv-5.10.0\\etc\\haarcascades\\haarcascade_frontalface_alt.xml")) {
    qDebug() << "无法加载分类器模型！";
  }
}

FaceRecognizer::FaceRecognizer(OnCaptureHook onCaptureHook,
                               OnDetectHook onDetectHook)
    : onCaptureHook(onCaptureHook), onDetectHook(onDetectHook), videoCapture(1) {

  if (!faceClassifier.load("D:\\opencv-5.10.0\\etc\\haarcascades\\haarcascade_frontalface_alt.xml")) {
    qDebug() << "无法加载分类器模型！";
  }
}

/**
 * @brief 启动人脸识别服务
 */
void FaceRecognizer::Start() {
  // QString url = "192.168.11.182:4747/video";
  // cv::Mat frame = cv::imread("/home/yu/codefield/ArmFaceID/LDH.jpg", 1);
  // videoCapture >> frame;
  // cv::namedWindow("test", cv::WINDOW_NORMAL);
  // cv::Mat firstFrame;
  // videoCapture.read(firstFrame);
  // cv::imshow("test", firstFrame);
  workThread = std::make_unique<std::thread>([&]{
    while (true) {
      cv::Mat frame;
      videoCapture.read(frame);
      if (frame.empty()) {
        continue;
      }

      std::vector<cv::Rect> faces;
      DetectFace(frame, faces);
      if (!faces.empty()) {
        auto face_rect = faces.at(0);
        cv::rectangle(frame, face_rect, cv::Scalar(0, 255, 255));
      }
      onCaptureHook(frame);
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
  });
  workThread->detach();
}

void FaceRecognizer::DetectFace(cv::Mat image, std::vector<cv::Rect> &faces) {
  cv::Mat image_gray;
  cv::cvtColor(image, image_gray, cv::COLOR_BGR2GRAY);
  cv::equalizeHist(image_gray, image_gray);

  faceClassifier.detectMultiScale(image_gray, faces);
}
