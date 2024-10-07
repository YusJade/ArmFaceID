#include "tableview_page.h"

#include <ElaScrollPageArea.h>
#include <qboxlayout.h>
#include <qgridlayout.h>
#include <qnamespace.h>
#include <qsqltablemodel.h>
#include <qwidget.h>

#include <QItemSelectionModel>

#include "Def.h"
#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPushButton.h"
#include "ElaTableView.h"
#include "face_database.h"

using namespace arm_face_id;

TableViewPage::TableViewPage() : db_conn_("QSQLITE", "db_arm_face_id.db") {
  InitPage();
}

void TableViewPage::InitPage() {
  ElaScrollPageArea* main_area = new ElaScrollPageArea(this);
  main_area->setContentsMargins(20, 20, 20, 20);
  main_area->setFixedSize(800, 475);
  // QWidget* main_widget = new QWidget;
  QGridLayout* main_layout = new QGridLayout(main_area);

  // setCustomWidget(main_widget);

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
  ElaPushButton* refresh_btn = new ElaPushButton("刷新");
  refresh_btn->setMaximumWidth(100);
  refresh_btn->setFixedHeight(30);
  ElaComboBox* search_combox = new ElaComboBox;
  search_combox->setMaximumWidth(100);
  search_combox->setFixedHeight(30);

  operation_layout->addWidget(search_input, 0, Qt::AlignLeft);
  operation_layout->addWidget(search_btn, 0, Qt::AlignLeft);
  // operation_layout->addWidget(search_combox, 1, Qt::AlignLeft);
  operation_layout->addWidget(delete_btn, 0, Qt::AlignLeft);
  operation_layout->addWidget(refresh_btn, 0, Qt::AlignLeft);

  operation_layout->addSpacing(10);
  operation_layout->setContentsMargins(0, 0, 0, 0);
  operation_layout->addStretch();

  main_layout->addLayout(operation_layout, 0, 0);
  main_layout->addWidget(table_view_, 1, 0, 1, 4);

  connect(refresh_btn, &ElaPushButton::clicked, this,
          [&] { table_model_->select(); });

  connect(delete_btn, &ElaPushButton::clicked, this, [&] {
    QItemSelectionModel* selection_model = table_view_->selectionModel();
    QModelIndexList selected_indexes = selection_model->selectedIndexes();

    // 创建一个集合来存储选中的行号
    QSet<int> selected_rows;
    foreach (const QModelIndex& index, selected_indexes) {
      selected_rows.insert(index.row());
    }

    // 删除选中的行（从最后一行开始删除，以避免索引变化）
    QList<int> sortedRows = selected_rows.values();
    std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());
    foreach (int row, sortedRows) {
      table_model_->removeRow(row);
    }

    // 提交更改到数据库
    if (!table_model_->submitAll()) {
      ElaMessageBar::warning(ElaMessageBarType::TopRight, "数据库", "操作失败",
                             3000);
    }
    table_model_->select();
  });
}