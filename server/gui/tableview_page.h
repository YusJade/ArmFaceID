#pragma once

#include <QSqlTableModel>

#include "./table_model/user_table_model.h"
#include "ElaScrollPage.h"
#include "ElaTableView.h"
#include "face_database.h"

namespace arm_face_id {

class TableViewPage : public ElaScrollPage {
 public:
  TableViewPage();
  void InitPage();

 private:
  ElaTableView* table_view_;
  table_view::UserTableModel* table_model_;
  data::DBConnection db_conn_;
};

}  // namespace arm_face_id