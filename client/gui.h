#ifndef DISPLAY_WIDGET_H
#define DISPLAY_WIDGET_H

#include <qmainwindow.h>

#include <QLabel>
#include <QWidget>
#include <memory>

#include <opencv2/imgproc.hpp>

#include "camera.h"
#include "face_processor.h"
#include "face_processor_listener.h"
#include "rpc_client.h"

namespace arm_face_id {

class GUI : public QMainWindow, public ICamera, public FaceProcessorListener {
 public:
  GUI();
  GUI(std::shared_ptr<RpcClient> rpc_client_ptr);

  [[deprecated]]
  GUI(const std::string& rpc_server_addr, FaceProcessor* processor);
  [[deprecated]]
  void OnFrameCaptured(cv::Mat frame) override;

  [[deprecated]]
  void OnImageCaptured(cv::Mat captureed_image) override;
  [[deprecated]]
  void OnFaceDetected(cv::Mat detected_image, cv::Rect face_rect) override;
  [[deprecated]]
  void OnFaceRecognized(RecognitionResult result) override;

  QWidget* Get();

 private:
  cv::Mat face_img_;
  FaceProcessor* processor_ = nullptr;
  std::shared_ptr<arm_face_id::RpcClient> rpc_client_;
  QWidget* main_widget_ = nullptr;
  QLabel* camera_frame_lbl_ = nullptr;
};
}  // namespace arm_face_id

#endif  // DISPLAY_WIDGET_H