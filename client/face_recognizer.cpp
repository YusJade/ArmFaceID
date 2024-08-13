#include "face_recognizer.h"

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QString>

// FaceRecognizer::FaceRecognizer()
//     : onCaptureHook(), onDetectHook(), videoCapture(1) {
//   // if (!faceClassifier.load(
//   //         "D:\\cmake-install-modules\\opencv-4.10."
//   //         "0\\etc\\haarcascades\\haarcascade_frontalface_alt_tree.xml")) {
//   //   QMessageBox::critical(nullptr, "错误",
//   //                         "发生了一个致命错误，程序即将关闭。");
//   //   QApplication::exit(1);
//   //   // qDebug() << "无法加载分类器模型！";
//   // }
// }

FaceRecognizer::FaceRecognizer(OnCaptureHook onCaptureHook,
                               OnDetectHook onDetectHook)
    : onCaptureHook(onCaptureHook),
      onDetectHook(onDetectHook),
      videoCapture("http://192.168.11.182:4747/video") {
  // seeta::ModelSetting fd_setting("fd_2_00.dat",
  //                                seeta::ModelSetting::Device::CPU);
  // face_detector = std::make_unique<seeta::FaceDetector>(fd_setting);

  if (!faceClassifier.load(
          "E:\\Downloads\\lbpcascade_frontalface_improved.xml")) {
    QMessageBox::critical(nullptr, "错误",
                          "发生了一个致命错误，程序即将关闭。");
    QApplication::exit(1);
    // qDebug() << "无法加载分类器模型！";
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
  workThread = std::make_unique<std::thread>([&] {
    while (true) {
      cv::Mat frame;
      videoCapture.read(frame);
      if (frame.empty()) {
        continue;
      }

      // seetaface 人脸检测
      // SeetaFaceInfoArray face_array = face_detector->detect(
      //     SeetaImageData{frame.cols, frame.rows, frame.channels(),
      //     frame.data});
      // if (face_array.size > 0) {
      //   auto face = face_array.data[0];
      //   auto seeta_face_rect = face.pos;
      //   cv::Rect cv_face_rect(seeta_face_rect.x, seeta_face_rect.y,
      //                         seeta_face_rect.width, seeta_face_rect.height);
      //   cv::rectangle(frame, cv_face_rect, cv::Scalar(0, 255, 255));
      //   // 通过 cv::Mat 的重载运算符截取出矩形部分
      //   onDetectHook(frame(cv_face_rect));
      // }

      // opencv 实现人脸检测
      std::vector<cv::Rect> faces;
      DetectFace(frame, faces);
      if (!faces.empty()) {
        auto face_rect = faces.at(0);
        cv::rectangle(frame, face_rect, cv::Scalar(0, 255, 255));
        // 通过 cv::Mat 的重载运算符截取出矩形部分
        onDetectHook(frame(face_rect));
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
