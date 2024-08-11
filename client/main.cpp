#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>

#include <grpcpp/grpcpp.h>

#include "face_recognizer.h"
#include "mainwindow.h"
#include "rpc_client.h"
#include "utils/utils.h"

constexpr char serverAddrUrl[] = "localhost:50051";

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  arm_face_id::RpcClient rpc_client(
      grpc::CreateChannel(serverAddrUrl, grpc::InsecureChannelCredentials()));

  auto onCaptureHook = [&](cv::Mat image) {
    w.SetDisplayLabelImg(utils::matToQImage(image));
  };
  auto onDetectHook = [&](cv::Mat face_img) {
    if (w.registerBtnState() == "识别") {
      cv::Mat face_img_res = rpc_client.RecognizeFace(face_img);
      w.SetFaceLabelImg(utils::matToQImage(face_img_res));
    } else {
      int64_t id = rpc_client.Register(face_img, "Test_Player");
      w.SetIdLabel(QString("{%1}").arg(id));
    }
  };
  FaceRecognizer faceRecognizer(onCaptureHook, onDetectHook);
  faceRecognizer.Start();

  return a.exec();
}
