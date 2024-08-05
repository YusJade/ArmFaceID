#include "face_recognizer.h"

#include <QString>

FaceRecognizer::FaceRecognizer() : onCaptureHook(), onDetectHook() {}

FaceRecognizer::FaceRecognizer(std::function<void()> onCaptureHook,
                               std::function<void()> onDetectHook)
    : onCaptureHook(onCaptureHook), onDetectHook(onDetectHook) {}

void FaceRecognizer::Start() {
    QString url = "192.168.11.182:4747/video";
    videoCapture.open(url.toStdString());
    cv::Mat frame;
    videoCapture >> frame;
    cv::imshow("test", 0);
}
