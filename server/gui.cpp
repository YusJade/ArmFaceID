#include "gui.h"

#include <qaction.h>
#include <qboxlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qstackedwidget.h>
#include <qtextedit.h>
#include <qtoolbar.h>
#include <qwidget.h>

#include <QGroupBox>
#include <QTextEdit>
#include <memory>

#include <spdlog/spdlog.h>

#include "engine.h"

arm_face_id::GUI::GUI(std::shared_ptr<arm_face_id::Engine> engine_ptr)
    : engine_ptr_(engine_ptr) {}

void arm_face_id::GUI::Init() {}

QWidget* arm_face_id::GUI::Get() {
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
  return main_widget_;
}

QWidget* arm_face_id::GUI::InitRpcWidget() {
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

QWidget* arm_face_id::GUI::InitRegisterWidget() {
  QWidget* main_widget = new QWidget;
  return main_widget;
}

void arm_face_id::GUI::OnFrameCaptured(cv::Mat frame) {
  spdlog::info("GUI: OnFrameCaptured");
}
