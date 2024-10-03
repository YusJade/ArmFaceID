#include "face_database.h"

#include <qbitarray.h>
#include <qbuffer.h>
#include <qglobal.h>
#include <qhashfunctions.h>
#include <qobjectdefs.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qstringview.h>
#include <qthread.h>
#include <qtsqlglobal.h>
#include <qtypes.h>

#include <QImage>
#include <algorithm>
#include <string>
#include <thread>
#include <utility>

#include <QtSql/QSqlError>

#include "fmt/bundled/core.h"
#include "function.h"
#include "spdlog.h"

using namespace arm_face_id::data;

constexpr const char* db_driver = "QSQLITE";
constexpr const char* db_name = "db_arm_face_id.db";

constexpr const char* sql_create_tb_usr =
    "CREATE TABLE tb_user(user_id INTEGER PRIMARY KEY AUTOINCREMENT, "
    "nick_name VARCHAR, "
    "face_img BLOB,"
    "email VARCHAR);";

constexpr const char* sql_insert_tb_usr =
    "INSERT INTO tb_user(user_id, nick_name, email, "
    "face_img) VALUES (NULL, ?, ?, ?);";

constexpr const char* sql_select_tb_usr =
    "SELECT user_id, nick_name, email, face_img FROM tb_user;";

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

FaceDataBase* FaceDataBase::instance_ = nullptr;

FaceDataBase& FaceDataBase::BuildAndReturn() {
  ASSERET_WITH_LOG("数据库单例已存在", !instance_);
  instance_ = new FaceDataBase();
  return *instance_;
}

FaceDataBase& FaceDataBase::GetInstance() {
  ASSERET_WITH_LOG("数据库单例未创建", instance_);
  return *instance_;
}

FaceDataBase::FaceDataBase() {}

bool FaceDataBase::InitDb() {
  QSqlDatabase::addDatabase("QSQLITE");

  DBConnection db_conn(db_driver, db_name);
  auto& db = db_conn.GetSqlDatabase();

  if (!db.open()) {
    // spdlog::error("无法打开数据库！:x : {}", db_.lastError().text());
    return false;
  }
  // QSqlQuery sql_query(db);
  auto& sql_query = db_conn.GetSqlQuery();

  QStringList table_list = db.tables();
  auto db_list = fmt::format("检测到数据表：");
  for (auto tb_name : table_list) {
    db_list += fmt::format("{}, ", tb_name);
  }
  spdlog::info(db_list);
  sql_query.prepare(sql_create_tb_usr);
  if (!sql_query.exec()) {
    spdlog::error("无法创建表格 tb_user : {} | {} | {}", db.lastError().text(),
                  db.lastError().driverText(), db.lastError().databaseText());
  }

  return true;
}

int FaceDataBase::AddUser(std::string nick_name, std::string email,
                          QImage face_img) {
  QByteArray img_bytes;
  QBuffer buffer(&img_bytes);
  buffer.open(QIODevice::WriteOnly);
  face_img.save(&buffer, "JPEG");
  buffer.close();

  DBConnection db_conn(db_driver, db_name);
  auto& sql_query = db_conn.GetSqlQuery();
  sql_query.prepare(
      "INSERT INTO tb_user(user_id, nick_name, email, "
      "face_img) VALUES (NULL, :nick_name, :email, :face_img);");
  // sql_query.prepare(
  //     "INSERT INTO tb_user(user_id, nick_name, email, "
  //     "face_img) VALUES (NULL, 'test', 'test', NULL)");

  sql_query.bindValue(":nick_name", QString::fromStdString(nick_name));
  sql_query.bindValue(":email", QString::fromStdString(email));
  sql_query.bindValue(":face_img", img_bytes);

  if (sql_query.exec()) {
    spdlog::info("已向数据库插入用户数据 ~ :  {} ({},{})",
                 sql_query.lastError().databaseText(), nick_name, email);
    return sql_query.lastInsertId().toInt();
  }
  spdlog::error("无法向数据库插入用户数据！: {} ({},{})",
                sql_query.lastError().text() +
                    sql_query.lastError().driverText() +
                    sql_query.lastError().databaseText(),
                nick_name, email);

  return -1;
}

bool FaceDataBase::RemoveUser(int user_id) { return 0; }

void FaceDataBase::LoadToCache() {
  auto db_conn = DBConnection(db_driver, db_name);
  auto& sql_query = db_conn.GetSqlQuery();
  if (!sql_query.exec(sql_select_tb_usr)) {
    spdlog::error("无法读取数据库！>_< : {}",
                  sql_query.lastError().text().toStdString());
    return;
  }

  users_.clear();
  while (sql_query.next()) {
    int user_id = sql_query.value(0).toInt();
    std::string nick_name = sql_query.value(1).toString().toStdString();
    std::string email = sql_query.value(2).toString().toStdString();

    auto img_qbytes = sql_query.value(3).toByteArray();
    std::vector<uint8_t> img_bytes(img_qbytes.size());
    std::copy(img_qbytes.begin(), img_qbytes.end(), img_bytes.begin());

    users_.push_back(User{user_id, nick_name, email, img_bytes});
  }

  spdlog::info("已将 {} 条用户数据从数据库加载到内存中 :O", users_.size());
  return;
}

User FaceDataBase::GetUserById(int id) {
  for (auto iter : users_) {
    if (iter.id == id) {
      return iter;
    }
  }
  return User();
}