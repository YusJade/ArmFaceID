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
  std::vector<uint8_t> face_img_bytes;
};

class DBConnection {
 public:
  DBConnection(std::string driver, std::string db_name);

  ~DBConnection();

  QSqlDatabase& GetSqlDatabase() { return db_; }
  QSqlQuery& GetSqlQuery() { return q_; }

 private:
  QSqlDatabase db_;
  QSqlQuery q_;
};

class FaceDataBase {
 public:
  static FaceDataBase& BuildAndReturn();
  static FaceDataBase& GetInstance();

 private:
  FaceDataBase();

 public:
  bool InitDb();
  int AddUser(std::string nick_name, std::string email, QImage face_img);
  bool RemoveUser(int user_id);

  void LoadToCache();

  User GetUserById(int id);
  const std::vector<User>& Users() const { return users_; }

 private:
  QSqlDatabase db_;
  QSqlQuery sql_query_;

  std::vector<User> users_;
  static FaceDataBase* instance_;
};
}  // namespace data
}  // namespace arm_face_id