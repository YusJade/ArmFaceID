#include "ela_gui.h"

#include <ElaPushButton.h>
#include <ElaScrollPage.h>
#include <ElaWidget.h>
#include <qboxlayout.h>
#include <qlabel.h>

#include <spdlog/spdlog.h>

#include "Def.h"

arm_face_id::ElaGUI::ElaGUI() {
  this->setUserInfoCardVisible(false);
  InitWindow();
}

void arm_face_id::ElaGUI::InitWindow() {
  // setCustomWidget(ElaAppBarType::CustomArea::LeftArea,
  //                 new QLabel("这是个标签~"));
  auto w = new ElaScrollPage();

  // auto l = new QHBoxLayout(w);
  w->layout()->addWidget(new QLabel("这是个标签~"));
  w->layout()->addWidget(new ElaPushButton("注册"));
  // w->setWindowButtonFlag(ElaAppBarType::ButtonType::CloseButtonHint, false);
  auto res = addPageNode("注册", w, 9);
  spdlog::warn("添加导航栏节点：{}", res);
}