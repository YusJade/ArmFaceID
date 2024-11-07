#pragma once

#include <qlabel.h>
#include <qmainwindow.h>

#include <QApplication>
#include <QStackedWidget>
#include <QWidget>
#include <memory>

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <opencv2/core/mat.hpp>

#include "engine.h"
#include "observer/core.h"
#include "seeta/CFaceInfo.h"
#include "seeta/Common/CStruct.h"

namespace arm_face_id {

using namespace treasure_chest;

class GUI : public QMainWindow, public pattern::Observer<cv::Mat> {
 public:
  GUI();

  void SetEngine(std::shared_ptr<FaceEngine> engine) { engine_ = engine; }

 private:
  void OnNotify(const cv::Mat &message);

  bool is_recognize_model = false;
  std::shared_ptr<arm_face_id::FaceEngine> engine_;
  SeetaFaceInfo current_face_info_;
  SeetaImageData current_face_image_;

 private:
  QWidget *central_widget;
  QWidget *horizontal_layout_widget;
  QHBoxLayout *horizontal_layout;
  QGroupBox *video_display_group_box;
  QWidget *video_display_layout_widget;
  QHBoxLayout *video_display_layout;
  QLabel *video_widget;
  QVBoxLayout *vertical_layout;
  QGroupBox *user_info_group_box;
  QWidget *user_info_layout_widget;
  QVBoxLayout *user_info_layout;
  QLabel *user_info_label;
  QGroupBox *settings_group_box;
  QWidget *settings_layout_widget;
  QVBoxLayout *settings_layout;
  QLabel *settings_label;
  QGridLayout *input_grid_layout;
  QLineEdit *email_line_edit;
  QLabel *email_label;
  QPushButton *submit_button;
  QLineEdit *username_date_line_edit;
  QLabel *recognition_date_label;
  QLabel *model_label;
  QRadioButton *model_recognize_radio_button;
  QRadioButton *model_register_radio_button;
  QLineEdit *recognition_date_line_edit;
  QLabel *user_name_label;
  QMenuBar *menu_bar;
  QStatusBar *status_bar;

 private:
  void SetupUi();
  void RetranslateUi();
  void SetupConnection();
  /**
   * @brief 切换至注册模式
   *
   */
  void SwitchRegisterModel();

  void SwitchRecognizeModel();
};
}  // namespace arm_face_id
