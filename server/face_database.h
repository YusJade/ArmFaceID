#pragma once

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>

namespace arm_face_id {
class FaceDataBase {
 public:
  FaceDataBase();

  bool InitDb();
  int AddUser(QString nick_name, QString email);
  bool RemoveUser(int user_id);

 private:
  QSqlDatabase db_;
};
}  // namespace arm_face_id