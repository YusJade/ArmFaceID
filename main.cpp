#include <QApplication>

#include "mainwindow.h"
#include "face_recognizer.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  FaceRecognizer faceRecognizer;
  faceRecognizer.Start();

  return a.exec();
}
