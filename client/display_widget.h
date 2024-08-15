#ifndef DISPLAY_WIDGET_H
#define DISPLAY_WIDGET_H

#include <QLabel>
#include <QWidget>

#include "face_processor.h"
#include "face_processor_listener.h"
#include "rpc_client.h"

namespace arm_face_id {

class DisplayWidget : public FaceProcessorListener {
 public:
  DisplayWidget() = delete;
  DisplayWidget(const std::string& rpc_server_addr, FaceProcessor* processor);

  void OnImageCaptured(cv::Mat captureed_image) override;
  void OnFaceDetected(cv::Mat detected_image, cv::Rect face_rect) override;
  void OnFaceRecognized(RecognitionResult result) override;

  QWidget* InitWidget();
  QWidget* widget();

 private:
  FaceProcessor* processor_ = nullptr;
  std::shared_ptr<arm_face_id::RpcClient> rpc_client_;
  QWidget* widget_ = nullptr;
  QLabel* capture_lbl_ = nullptr;
  QLabel* face_lbl_ = nullptr;
  QLabel* name_lbl_ = nullptr;
  QLabel* id_lbl_ = nullptr;
};

}  // namespace arm_face_id

#endif  // DISPLAY_WIDGET_H