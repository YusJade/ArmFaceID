#pragma once

#include <qglobal.h>
#include <qimage.h>
#include <qstringview.h>

#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>

namespace arm_face_id {

namespace data {

struct User {
  int id = -1;
  std::string user_name;
  std::string email;
  QImage profile_pic;
  QImage face_img;

  User() = default;
};

class DBConnection {
  using UserFeature = std::pair<User, std::vector<float>>;

 public:
  DBConnection(std::string driver = "QSQLITE",
               std::string db_name = "db_arm_face_id.db");

  ~DBConnection();

  static bool InitializeDatabase();
  static DBConnection& GetConnection();

  int InsertUser(const data::User&);
  bool DeleteUser(int user_id);
  std::vector<User> SelectAllUser();
  User SelectUserById(int id);
  [[deprecated]] void LoadToCache();
  [[deprecated]] const std::vector<UserFeature>& Users() const {
    return users_;
  }

 private:
  QSqlDatabase db_;
  QSqlQuery q_;

  QSqlDatabase& GetSqlDatabase() { return db_; }
  QSqlQuery& GetSqlQuery() { return q_; }

  std::vector<UserFeature> users_;

  static std::unordered_map<std::thread::id, DBConnection> connection_pool;
};

}  // namespace data
}  // namespace arm_face_id
