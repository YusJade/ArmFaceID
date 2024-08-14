#ifndef FACE_PROCESSOR_LISTENER_H
#define FACE_PROCESSOR_LISTENER_H

#include <opencv2/opencv.hpp>

namespace arm_face_id {

/**
 * @brief 用于处理人脸处理事件的接口。
 *
 * FaceProcessorListener 接口定义了处理摄像捕获、人脸检测、人脸识别
 * 相关事件的方法，实现该接口以响应这些特定的事件。
 *
 * OnImageCaptured 方法在新的画面帧被捕获时触发。
 * OnFaceDetected 方法在捕捉到人脸图像时触发。
 * OnFaceRecognized 方法在人脸信息被识别时触发。
 */
class FaceProcessorListener {
 public:
  struct RecognitionResult {
    cv::Mat face_image;
    int64_t id;
    std::string name;
  };

  FaceProcessorListener() = default;
  ~FaceProcessorListener() = default;

  /**
   * @brief 当读取到新的画面帧的回调方法。
   *
   * @param captureed_image 读取的新画面帧
   */
  virtual void OnImageCaptured(cv::Mat captureed_image) = 0;
  /**
   * @brief 当检测到画面帧中有人脸时的回调方法。
   *
   * @param detected_image 人脸图像
   * @param face_rect 人脸图像在画面帧中的矩形
   */
  virtual void OnFaceDetected(cv::Mat detected_image, cv::Rect face_rect) = 0;
  /**
   * @brief 当人脸被识别出结果时的回调方法。
   *
   * @param result 识别结果，即人脸相关的信息
   */
  virtual void OnFaceRecognized(RecognitionResult result) = 0;

 private:
  friend class FaceProcessor;
};

}  // namespace arm_face_id

#endif  // FACE_PROCESSOR_LISTENER_H