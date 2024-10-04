#include "tableview_page.h"

#include <qgridlayout.h>
#include <qsqltablemodel.h>
#include <qwidget.h>

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
  main_layout->addWidget(table_view_, 1, 1);

  setCustomWidget(main_widget);
}