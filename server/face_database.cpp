#include "face_database.h"

#include <qglobal.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>

#include <string>

#include <QtSql/QSqlError>

#include "fmt/bundled/core.h"
#include "spdlog.h"

using namespace arm_face_id;

constexpr const char* sql_create_tb_usr =
    "create table tb_user(user_id integer primary key, nick_name varchar, "
    "email varchar);";

FaceDataBase::FaceDataBase() {}

bool FaceDataBase::InitDb() {
  db_ = QSqlDatabase::addDatabase("QSQLITE");
  db_.setDatabaseName("db_arm_face_id");

  if (!db_.open()) {
    spdlog::error("无法打开数据库！:x : {}",
                  db_.lastError().text().toStdString());
    return false;
  }
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

int AddUser(QString nick_name, QString email) { return 0; }

bool RemoveUser(int user_id) { return 0; }