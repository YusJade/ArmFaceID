#include <QApplication>

#include "mainwindow.h"
#include "face_recognizer.h"
#include "utils/utils.h"

int main(int argc, char *argv[]) {

  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  auto onCaptureHook = [&](cv::Mat image){
    w.SetDisplayLabelImg(utils::matToQImage(image));
  };
  auto onDetectHook = [&]{ };
  FaceRecognizer faceRecognizer(onCaptureHook, onDetectHook);
  faceRecognizer.Start();


  return a.exec();
}
