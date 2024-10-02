#pragma once

#include <qstringview.h>

#include <string>
#include <vector>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>

namespace arm_face_id {

namespace data {

struct User {
  int id;
  std::string nick_name;
  std::string email;
  std::string face_img_bytes;
};

class FaceDataBase {
 public:
  FaceDataBase();

  bool InitDb();
  int AddUser(std::string nick_name, std::string email, QImage face_img);
  bool RemoveUser(int user_id);

  void LoadToCache();

  const std::vector<User>& Users() const { return users_; }

 private:
  QSqlDatabase db_;
  QSqlQuery sql_query_;

  std::vector<User> users_;
};
}  // namespace data
}  // namespace arm_face_id