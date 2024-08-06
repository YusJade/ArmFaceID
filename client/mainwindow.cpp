#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::MainWindow){

  ui->setupUi(this);
}

MainWindow::~MainWindow() {

  delete ui;
}

void MainWindow::SetDisplayLabelImg(QImage image) {

  ui->imageLabel->setPixmap(QPixmap::fromImage(image));
}
