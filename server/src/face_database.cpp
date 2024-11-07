#include "face_database.h"

#include <qbitarray.h>
#include <qbuffer.h>
#include <qglobal.h>
#include <qhashfunctions.h>
#include <qiodevicebase.h>
#include <qobjectdefs.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qstringview.h>
#include <qthread.h>
#include <qtsqlglobal.h>
#include <qtypes.h>

#include <QImage>
#include <cstring>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <QtSql/QSqlError>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include "utils/base.h"

using namespace arm_face_id::data;

constexpr const char* db_driver = "QSQLITE";
constexpr const char* db_name = "db_arm_face_id.db";

constexpr const char* sql_create_tb_usr =
    "CREATE TABLE tb_user(user_id INTEGER PRIMARY KEY AUTOINCREMENT, "
    "user_name VARCHAR, "
    "face_img BLOB,"
    "email VARCHAR,"
    "profile_pic BLOB);";

constexpr const char* sql_insert_tb_usr =
    "INSERT INTO tb_user(user_id, user_name, email, "
    "face_img) VALUES (NULL, ?, ?, ?);";

constexpr const char* sql_select_tb_usr =
    "SELECT user_id, user_name, face_img, email, profile_pic FROM tb_user;";

constexpr const char* sql_select_tb_usr_by_id =
    "SELECT user_id, user_name, face_img, email, profile_pic FROM tb_user "
    "WHERE user_id = "
    "?;";

std::unordered_map<std::thread::id, DBConnection> DBConnection::connection_pool;

DBConnection& DBConnection::GetConnection() {
  std::thread::id cur_thread_id = std::this_thread::get_id();
  if (connection_pool.count(cur_thread_id)) {
    return connection_pool.at(cur_thread_id);
  }
  connection_pool.insert_or_assign(cur_thread_id, DBConnection());

  return connection_pool.at(cur_thread_id);
}

DBConnection::DBConnection(std::string driver, std::string db_name)
    : db_(QSqlDatabase::addDatabase(
          QString::fromStdString(driver),
          QString::number((quint64)QThread::currentThreadId()))) {
  db_.setDatabaseName(QString::fromStdString(db_name));
  if (!db_.open()) {
    spdlog::error("无法打开数据库！:x : {}", db_.lastError().text());
  }
  spdlog::info("成功打开数据库: {}", db_.databaseName());
  q_ = QSqlQuery(db_);
}

DBConnection::~DBConnection() { q_.finish(); }

bool DBConnection::InitializeDatabase() {
  DBConnection db_conn(db_driver, db_name);
  auto& db = db_conn.GetSqlDatabase();
  auto& sql_query = db_conn.GetSqlQuery();
  if (!db.open()) {
    spdlog::error("数据库初始化失败：无法打开数据库！:x : {}",
                  db.lastError().text());
    return false;
  }
  spdlog::info("成功打开数据库：正在初始化。");
  QStringList table_list = db.tables();
  auto db_list = fmt::format("存在数据表：");
  for (auto tb_name : table_list) {
    db_list += fmt::format("{}, ", tb_name);
  }
  spdlog::info(db_list);
  sql_query.prepare(sql_create_tb_usr);
  if (!sql_query.exec()) {
    spdlog::error("无法创建表格 tb_user : {} \n {} \n {}",
                  db.lastError().text(), db.lastError().driverText(),
                  db.lastError().databaseText());
  }

  return true;
}

int DBConnection::InsertUser(const User& user) {
  QByteArray face_img_bytes;
  QDataStream face_img_stream(&face_img_bytes, QIODeviceBase::WriteOnly);
  face_img_stream << user.face_img;

  QByteArray profile_pic_bytes;
  QDataStream profile_pic_stream(&profile_pic_bytes, QIODeviceBase::WriteOnly);
  profile_pic_stream << user.profile_pic;

  DBConnection db_conn(db_driver, db_name);
  auto& sql_query = db_conn.GetSqlQuery();
  sql_query.prepare(
      "INSERT INTO tb_user(user_id, user_name, email, profile_pic, "
      "face_img) VALUES (NULL, :user_name, :email, :profile_pic, "
      ":face_img);");
  // sql_query.prepare(
  //     "INSERT INTO tb_user(user_id, nick_name, email, "
  //     "face_img) VALUES (NULL, 'test', 'test', NULL)");

  sql_query.bindValue(":user_name", QString::fromStdString(user.user_name));
  sql_query.bindValue(":email", QString::fromStdString(user.email));
  sql_query.bindValue(":profile_pic", profile_pic_bytes);
  sql_query.bindValue(":face_img", face_img_bytes);

  if (sql_query.exec()) {
    spdlog::info("已向数据库插入用户数据 ~ :  {} ({},{})",
                 sql_query.lastError().databaseText(), user.user_name,
                 user.email);
    return sql_query.lastInsertId().toInt();
  }
  spdlog::error("无法向数据库插入用户数据！: {} ({},{})",
                sql_query.lastError().text() +
                    sql_query.lastError().driverText() +
                    sql_query.lastError().databaseText(),
                user.user_name, user.email);

  return -1;
}

bool DBConnection::DeleteUser(int user_id) { return 0; }

User DBConnection::SelectUserById(int id) {
  QSqlQuery& q = GetSqlQuery();

  q.prepare(sql_select_tb_usr_by_id);
  q.bindValue(0, id);
  if (!q.exec()) {
    spdlog::error("无法读取数据库！>_< : {}",
                  q.lastError().text().toStdString());
    return User{-1};
  }

  User user;
  while (q.next()) {
    int user_id = q.value(0).toInt();
    std::string user_name = q.value(1).toString().toStdString();

    QByteArray face_img_qbytes = q.value(2).toByteArray();
    QImage face_img;
    face_img.loadFromData(face_img_qbytes);

    std::string email = q.value(3).toString().toStdString();

    QByteArray profile_pic_qbytes = q.value(4).toByteArray();
    QImage profile_pic;
    profile_pic.loadFromData(profile_pic_qbytes);

    user.id = user_id;
    user.email = email;
    user.user_name = user_name;
    user.profile_pic = profile_pic;
    user.face_img = face_img;
  }
  return user;
}

std::vector<User> DBConnection::SelectAllUser() {
  q_.prepare(sql_select_tb_usr);
  if (!q_.exec()) {
    spdlog::error("无法读取数据库！>_< : {}",
                  q_.lastError().text().toStdString());
    return std::vector<User>();
  }

  std::vector<User> res;
  while (q_.next()) {
    User user;
    int user_id = q_.value(0).toInt();
    std::string user_name = q_.value(1).toString().toStdString();

    QByteArray face_img_qbytes = q_.value(2).toByteArray();
    QImage face_img;
    QDataStream face_stream(face_img_qbytes);
    face_stream >> face_img;
    // face_img.loadFromData(face_img_qbytes);

    std::string email = q_.value(3).toString().toStdString();

    QByteArray profile_pic_qbytes = q_.value(4).toByteArray();
    QImage profile_pic;
    QDataStream profile_stream(profile_pic_qbytes);
    profile_stream >> profile_pic;
    // profile_pic.loadFromData(profile_pic_qbytes);

    user.id = user_id;
    user.email = email;
    user.user_name = user_name;
    user.profile_pic = profile_pic;
    user.face_img = face_img;
    spdlog::info(
        "Database: loaded user id={} username={}, face_img=[{}x{}], "
        "profile_pic=[{}x{}]",
        user.id, user.user_name, user.face_img.width(), user.face_img.height(),
        user.profile_pic.width(), user.profile_pic.height());
    res.push_back(user);
  }
  return res;
}
