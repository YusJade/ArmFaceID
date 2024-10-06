#include "tableview_page.h"

#include <qboxlayout.h>
#include <qgridlayout.h>
#include <qnamespace.h>
#include <qsqltablemodel.h>
#include <qwidget.h>

#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaTableView.h"
#include "face_database.h"

using namespace arm_face_id;

TableViewPage::TableViewPage() : db_conn_("QSQLITE", "db_arm_face_id.db") {
  InitPage();
}

void TableViewPage::InitPage() {
  QWidget* main_widget = new QWidget;
  QGridLayout* main_layout = new QGridLayout(main_widget);

  table_model_ =
      new table_view::UserTableModel(this, db_conn_.GetSqlDatabase());
  table_model_->setTable("tb_user");
  table_model_->select();
  table_view_ = new ElaTableView;
  table_view_->setModel(table_model_);
  table_view_->adjustSize();

  QHBoxLayout* operation_layout = new QHBoxLayout;

  ElaLineEdit* search_input = new ElaLineEdit;
  search_input->setMaximumWidth(100);
  search_input->setFixedHeight(30);
  ElaPushButton* search_btn = new ElaPushButton("搜索");
  search_btn->setMaximumWidth(100);
  search_btn->setFixedHeight(30);
  ElaPushButton* delete_btn = new ElaPushButton("删除行");
  delete_btn->setMaximumWidth(100);
  delete_btn->setFixedHeight(30);
  ElaComboBox* search_combox = new ElaComboBox;
  search_combox->setMaximumWidth(100);
  search_combox->setFixedHeight(30);

  operation_layout->addWidget(search_input, 0, Qt::AlignLeft);
  operation_layout->addWidget(search_btn, 0, Qt::AlignLeft);
  // operation_layout->addWidget(search_combox, 1, Qt::AlignLeft);
  operation_layout->addWidget(delete_btn, 0, Qt::AlignLeft);

  operation_layout->addSpacing(10);
  operation_layout->setContentsMargins(0, 0, 0, 0);
  operation_layout->addStretch();

  main_layout->addLayout(operation_layout, 0, 0);
  main_layout->addWidget(table_view_, 1, 0, 1, 4);

  setCustomWidget(main_widget);
}