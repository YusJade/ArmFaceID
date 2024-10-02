#include "face_database.h"

#include <qbitarray.h>
#include <qbuffer.h>
#include <qglobal.h>
#include <qobjectdefs.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qstringview.h>

#include <QImage>
#include <string>

#include <QtSql/QSqlError>

#include "fmt/bundled/core.h"
#include "function.h"
#include "spdlog.h"

using namespace arm_face_id::data;

constexpr const char* sql_create_tb_usr =
    "CREATE TABLE tb_user(user_id INTERGER PRIMARY KEY AUTOINCREMENT, "
    "nick_name VARCHAR, "
    "face_img BLOB"
    "email VARCHAR);";

constexpr const char* sql_insert_tb_usr =
    "INSERT INTO tb_user(nick_name, email,"
    "face_img) VALUES (:nick_name, :email, :face_img);";

constexpr const char* sql_select_tb_usr = "SELECT * FROM tb_user;";

FaceDataBase::FaceDataBase() {}

bool FaceDataBase::InitDb() {
  db_ = QSqlDatabase::addDatabase("QSQLITE");
  db_.setDatabaseName("db_arm_face_id");

  if (!db_.open()) {
    spdlog::error("无法打开数据库！:x : {}", db_.lastError().text());
    return false;
  }
  sql_query_ = QSqlQuery(db_);
  spdlog::info("成功打开数据库: {}", db_.databaseName().toStdString());

  QStringList table_list = db_.tables();
  auto db_list = fmt::format("检测到数据表：");
  for (auto tb_name : table_list) {
    db_list += fmt::format("{}, ", tb_name.toStdString());
  }
  spdlog::info(db_list);

  QSqlQuery sql_query;
  if (!sql_query.exec(QLatin1String(sql_create_tb_usr))) {
    spdlog::error("无法执行 sql : {}", db_.lastError().text().toStdString());
  }

  return true;
}

int FaceDataBase::AddUser(std::string nick_name, std::string email,
                          QImage face_img) {
  QByteArray img_bytes;
  QBuffer buffer(&img_bytes);
  buffer.open(QIODevice::WriteOnly);
  face_img.save(&buffer, "JPEG");

  sql_query_.prepare(sql_insert_tb_usr);
  sql_query_.bindValue(0, QString::fromStdString(nick_name));
  sql_query_.bindValue(1, QString::fromStdString(email));
  sql_query_.bindValue(2, img_bytes);

  if (sql_query_.exec()) {
    return sql_query_.lastInsertId().toInt();
    spdlog::info("已向数据库插入用户数据 ~ : {} ({},{})",
                 sql_query_.lastError().databaseText().toStdString(), nick_name,
                 email);
  }
  spdlog::error("无法向数据库插入用户数据！: {} ({},{})",
                sql_query_.lastError().databaseText().toStdString(), nick_name,
                email);

  return -1;
}

bool FaceDataBase::RemoveUser(int user_id) { return 0; }

void FaceDataBase::LoadToCache() {
  sql_query_.prepare(sql_select_tb_usr);
  if (!sql_query_.exec()) {
    spdlog::error("无法读取数据库！>_< : {}",
                  sql_query_.lastError().text().toStdString());
    return;
  }

  users_.clear();
  while (sql_query_.next()) {
    int user_id = sql_query_.value(0).toInt();
    std::string nick_name = sql_query_.value(1).toString().toStdString();
    std::string email = sql_query_.value(2).toString().toStdString();
    std::string img_bytes = sql_query_.value(3).toByteArray().toStdString();
    users_.push_back(User{user_id, nick_name, email, img_bytes});
  }

  spdlog::info("已将 {} 条用户数据从数据库加载到内存中 :O", users_.size());
  return;
}