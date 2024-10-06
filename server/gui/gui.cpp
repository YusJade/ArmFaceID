#include "gui.h"

#include <qaction.h>
#include <qboxlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qstackedwidget.h>
#include <qtextedit.h>
#include <qtextformat.h>
#include <qtoolbar.h>
#include <qwidget.h>

#include <QFileDialog>
#include <QGroupBox>
#include <QTextEdit>
#include <functional>
#include <memory>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <spdlog/spdlog.h>

#include "engine.h"
#include "function.h"

using namespace arm_face_id;

GUI::GUI(std::shared_ptr<arm_face_id::FaceDetectorServer> engine_ptr)
    : engine_ptr_(engine_ptr), on_frame_captured_callback_([](cv::Mat) {}) {}

void GUI::Init() {}

QWidget* GUI::Get() {
  if (main_widget_) return main_widget_;
  spdlog::info("Initializing Qt GUI...");
  main_widget_ = new QWidget;
  auto main_layout = new QVBoxLayout(main_widget_);
  auto toolbar = new QToolBar(main_widget_);
  main_layout->addWidget(toolbar);
  main_widget_->setLayout(main_layout);

  QAction* rpc_action = toolbar->addAction("rpc");
  QAction* register_action = toolbar->addAction("register");
  QAction* load_action = toolbar->addAction("load");
  QAction* save_action = toolbar->addAction("save");

  rpc_widget_ = InitRpcWidget();
  register_widget_ = InitRegisterWidget();

  stacked_widget_ = new QStackedWidget(main_widget_);
  main_layout->addWidget(stacked_widget_);
  stacked_widget_->addWidget(rpc_widget_);
  stacked_widget_->addWidget(register_widget_);

  QObject::connect(rpc_action, &QAction::triggered,
                   [&] { stacked_widget_->setCurrentWidget(rpc_widget_); });
  QObject::connect(register_action, &QAction::triggered, [&] {
    stacked_widget_->setCurrentWidget(register_widget_);
  });
  QObject::connect(save_action, &QAction::triggered, [&] {
    std::string file_path = QFileDialog::getSaveFileName().toStdString();
    engine_ptr_->Save(file_path);
  });
  QObject::connect(load_action, &QAction::triggered, [&] {
    std::string file_path = QFileDialog::getOpenFileName().toStdString();
    engine_ptr_->Load(file_path);
  });

  return main_widget_;
}

QWidget* GUI::InitRpcWidget() {
  QWidget* main_widget = new QWidget;
  QGroupBox* log_groupbox = new QGroupBox(main_widget);
  log_groupbox->setTitle("rpc log");
  QHBoxLayout* main_layout = new QHBoxLayout(main_widget);
  main_widget->setLayout(main_layout);
  main_layout->addWidget(log_groupbox);

  QTextEdit* log_textedit = new QTextEdit(main_widget);
  log_textedit->setReadOnly(true);
  QVBoxLayout* log_layout = new QVBoxLayout(main_widget);
  log_groupbox->setLayout(log_layout);
  log_layout->addWidget(log_textedit);

  QGroupBox* face_groupbox = new QGroupBox(main_widget);
  face_groupbox->setTitle("face");
  QVBoxLayout* face_layout = new QVBoxLayout(main_widget);
  QLabel* img_label = new QLabel(main_widget);
  // img_label->setFixedSize(2.5 * 30, 3.5 * 30);
  QTextEdit* face_textedit = new QTextEdit(main_widget);
  face_textedit->setReadOnly(true);
  // add Widgets
  face_layout->addWidget(img_label);
  face_layout->addWidget(face_textedit);
  // set strech factor
  face_layout->setStretchFactor(img_label, 3);
  face_layout->setStretchFactor(face_textedit, 1);
  face_groupbox->setLayout(face_layout);
  main_layout->addWidget(face_groupbox);

  main_layout->setStretchFactor(face_groupbox, 1);
  main_layout->setStretchFactor(log_groupbox, 5);

  return main_widget;
}

QWidget* GUI::InitRegisterWidget() {
  QWidget* main_widget = new QWidget;
  QGroupBox* log_groupbox = new QGroupBox(main_widget);
  log_groupbox->setTitle("register log");
  QHBoxLayout* main_layout = new QHBoxLayout(main_widget);
  main_widget->setLayout(main_layout);
  main_layout->addWidget(log_groupbox);

  QTextEdit* log_textedit = new QTextEdit(main_widget);
  log_textedit->setReadOnly(true);
  QVBoxLayout* log_layout = new QVBoxLayout(main_widget);
  log_groupbox->setLayout(log_layout);
  log_layout->addWidget(log_textedit);

  QGroupBox* face_groupbox = new QGroupBox(main_widget);
  face_groupbox->setTitle("face");
  QVBoxLayout* face_layout = new QVBoxLayout(main_widget);
  QLabel* img_label = new QLabel(main_widget);
  img_label->setScaledContents(true);
  // img_label->setFixedSize(2.5 * 30, 3.5 * 30);
  QPushButton* register_button = new QPushButton(main_widget);
  register_button->setText("register");

  QObject::connect(register_button, &QPushButton::clicked,
                   [this] { engine_ptr_->RegisterFace(captured_face_mat_); });

  // add Widgets
  face_layout->addWidget(img_label);
  face_layout->addStretch(4);
  face_layout->addWidget(register_button);
  // set strech factor
  face_layout->setStretchFactor(img_label, 3);
  face_layout->setStretchFactor(register_button, 1);
  face_groupbox->setLayout(face_layout);
  main_layout->addWidget(face_groupbox);

  main_layout->setStretchFactor(face_groupbox, 1);
  main_layout->setStretchFactor(log_groupbox, 5);

  on_frame_captured_callback_ =
      std::function<void(cv::Mat)>([img_label, this](cv::Mat frame) {
        captured_face_mat_ = frame;
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        QPixmap pixmap = QPixmap::fromImage(utils::mat_to_qimage(frame));
        pixmap = pixmap.scaled(img_label->size(), Qt::KeepAspectRatio,
                               Qt::SmoothTransformation);
        img_label->setPixmap(pixmap);
      });

  return main_widget;
}
