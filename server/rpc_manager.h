#ifndef RPC_MANAGER_H
#define RPC_MANAGER_H

#include <QLabel>
#include <QStackedWidget>
#include <QWidget>
#include <mutex>

#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>
#include <opencv2/opencv.hpp>

#include "face_engine.h"

#include "face_network.grpc.pb.h"
#include "face_network.pb.h"

namespace arm_face_id {

class RpcManagerImpl final : public RpcManager::Service {
 public:
  RpcManagerImpl();
  ~RpcManagerImpl();

  grpc::Status RecognizeFace(grpc::ServerContext* context,
                             const RecognizeRequest* request,
                             RecognizeResult* response) override;

  grpc::Status Register(grpc::ServerContext* context,
                        const RegisterRequest* request,
                        RegisterResult* response) override;

  QWidget* InitRpcWidget();
  QWidget* Widget();
  QWidget* InitRegisterWidget();

 private:
  std::unique_ptr<seeta::FaceEngine> face_engine_ptr;
  std::mutex mutex_;
  QWidget* rpc_widget_;
  QWidget* display_widget_;
  QWidget* register_widget_;
  QStackedWidget* stacked_widget_;
  QLabel* img_lbl_;
  QLabel* cam_img_lbl_;
  ::cv::CascadeClassifier face_classifier_;
  ::cv::Mat native_register_img_;
};

}  // namespace arm_face_id

#endif  // RPC_MANAGER_H