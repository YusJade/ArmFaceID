#include "mainwindow.h"

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  connect(ui->registerBtn, &QPushButton::clicked, [this]() {
    if (ui->registerBtn->text() == "注册") {
      ui->registerBtn->setText("识别");
    } else {
      ui->registerBtn->setText("注册");
    }
  });
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::SetDisplayLabelImg(QImage image) {
  ui->imageLabel->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::SetFaceLabelImg(QImage image) {
  ui->faceLabel->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::SetIdLabel(QString text) { ui->idLabel->setText(text); }

QString MainWindow::registerBtnState() const { return ui->registerBtn->text(); }
