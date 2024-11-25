#include "database/mysql.h"

#include <qglobal.h>
#include <qimage.h>
#include <qiodevice.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qsqlquery.h>
#include <qstringview.h>

#include <spdlog/spdlog.h>

using namespace arm_face_id;

MySqlSchema::MySqlSchema(string hostname, int port, string db, string user_name,
                         string password) {
  db_ = QSqlDatabase::addDatabase("QMYSQL");
  db_.setHostName(QString::fromStdString(hostname));
  db_.setPort(port);
  db_.setDatabaseName(QString::fromStdString(hostname));
  db_.setUserName(QString::fromStdString(hostname));
  db_.setPassword(QString::fromStdString(hostname));

  if (!db_.open()) {
    SPDLOG_ERROR("Failed to open mysql database: {}:{}.", hostname, port);
  } else {
    SPDLOG_INFO("Successfully opened mysql database: {}:{}.", hostname, port);
  }
}

bool MySqlSchema::SaveUser(const User &saved_user) {
  QSqlQuery q;
  q.prepare(
      R"(INSERT INTO tb_user VALUES
      (DEFAULT, :user_name, :email, :profile_pic, :face_img))");

  QByteArray face_img_bytearray, profile_pic_bytearray;
  QDataStream face_img_stream(&face_img_bytearray, QIODevice::WriteOnly);
  QDataStream profile_pic_stream(&profile_pic_bytearray, QIODevice::WriteOnly);

  profile_pic_stream << saved_user.profile_pic;
  face_img_stream << saved_user.face_img;

  q.bindValue(":user_name", QString::fromStdString(saved_user.user_name));
  q.bindValue(":email", QString::fromStdString(saved_user.email));
  q.bindValue(":profile_pic", profile_pic_bytearray);
  q.bindValue(":face_img", face_img_bytearray);

  if (q.exec()) {
    SPDLOG_INFO("Inserted into database: User({}, {}, {}, {}, {}, {}).",
                saved_user.user_id, saved_user.user_name, saved_user.email,
                !saved_user.profile_pic.isNull(),
                !saved_user.face_img.isNull());
    return true;
  }

  SPDLOG_ERROR("Failed to insert into database.");

  return false;
}

bool MySqlSchema::UpdateUser(int user_id,
                             std::function<void(User &)> update_func) {
  QSqlQuery select_query;
  select_query.prepare(R"(
        SELECT id, user_name, email, profile_pic, face_img
        FROM tb_user
        WHERE id = :user_id)");
  select_query.bindValue(":user_id", user_id);

  if (!select_query.exec() || !select_query.next()) {
    SPDLOG_ERROR("Failed to fetch User({}) from the database: {}", user_id,
                 select_query.lastError().text().toStdString());
    return false;
  }

  User user;
  user.user_id = select_query.value(0).toInt();
  user.user_name = select_query.value(1).toString().toStdString();
  user.email = select_query.value(2).toString().toStdString();

  QByteArray profile_pic_bytearray = select_query.value(3).toByteArray();
  QByteArray face_img_bytearray = select_query.value(4).toByteArray();

  QDataStream profile_pic_stream(&profile_pic_bytearray, QIODevice::ReadOnly);
  QDataStream face_img_stream(&face_img_bytearray, QIODevice::ReadOnly);

  profile_pic_stream >> user.profile_pic;
  face_img_stream >> user.face_img;

  SPDLOG_INFO("Fetched User({}) for update.", user.user_id);

  update_func(user);

  QSqlQuery update_query;
  update_query.prepare(R"(
        UPDATE tb_user
        SET username = :user_name, email = :email,
            profile_pic = :profile_pic, face_img = :face_img
        WHERE id = :user_id)");

  QByteArray new_profile_pic_bytearray, new_face_img_bytearray;
  QDataStream new_profile_pic_stream(&new_profile_pic_bytearray,
                                     QIODevice::WriteOnly);
  QDataStream new_face_img_stream(&new_face_img_bytearray,
                                  QIODevice::WriteOnly);

  new_profile_pic_stream << user.profile_pic;
  new_face_img_stream << user.face_img;

  update_query.bindValue(":user_name", QString::fromStdString(user.user_name));
  update_query.bindValue(":email", QString::fromStdString(user.email));
  update_query.bindValue(":profile_pic", new_profile_pic_bytearray);
  update_query.bindValue(":face_img", new_face_img_bytearray);

  if (update_query.exec()) {
    SPDLOG_INFO("Successfully updated User({}).", user.user_id);
    return true;
  }

  SPDLOG_ERROR("Failed to update User({}): {}", user.user_id,
               update_query.lastError().text().toStdString());
  return false;
}

User MySqlSchema::GetUserByID(int user_id) {
  QSqlQuery q;
  q.prepare(R"(
        SELECT id, username, email, profile_pic, face_img
        FROM tb_user
        WHERE id = :user_id)");

  q.bindValue(":user_id", user_id);

  if (q.exec() && q.next()) {
    User user;
    user.user_id = q.value(0).toInt();
    user.user_name = q.value(1).toString().toStdString();
    user.email = q.value(2).toString().toStdString();

    QByteArray profile_pic_bytearray = q.value(3).toByteArray();
    QByteArray face_img_bytearray = q.value(4).toByteArray();

    QDataStream profile_pic_stream(&profile_pic_bytearray, QIODevice::ReadOnly);
    QDataStream face_img_stream(&face_img_bytearray, QIODevice::ReadOnly);

    profile_pic_stream >> user.profile_pic;
    face_img_stream >> user.face_img;

    SPDLOG_INFO("Fetched User({}) from the database.", user.user_id);
    return user;
  }

  SPDLOG_ERROR("User with ID {} not found.", user_id);
  return {};
}

bool MySqlSchema::RemoveUser(int user_id) {
  QSqlQuery q;
  q.prepare("DELETE FROM tb_user WHERE id = :user_id");
  q.bindValue(":user_id", user_id);

  if (q.exec()) {
    SPDLOG_INFO("Successfully removed User({}) from the database.", user_id);
    return true;
  }

  SPDLOG_ERROR("Failed to remove User({}) from the database.", user_id);
  return false;
}

std::vector<User> MySqlSchema::ListAllUsers() {
  QSqlQuery q;
  q.prepare("SELECT id, user_name, email, profile_pic, face_img FROM tb_user");

  std::vector<User> users;

  if (q.exec()) {
    while (q.next()) {
      User user;
      user.user_id = q.value(0).toInt();
      user.user_name = q.value(1).toString().toStdString();
      user.email = q.value(2).toString().toStdString();

      QByteArray profile_pic_bytearray = q.value(3).toByteArray();
      QByteArray face_img_bytearray = q.value(4).toByteArray();

      QDataStream profile_pic_stream(&profile_pic_bytearray,
                                     QIODevice::ReadOnly);
      QDataStream face_img_stream(&face_img_bytearray, QIODevice::ReadOnly);

      profile_pic_stream >> user.profile_pic;
      face_img_stream >> user.face_img;

      users.push_back(user);
    }

    SPDLOG_INFO("Fetched {} users from the database.", users.size());
  } else {
    SPDLOG_ERROR("Failed to fetch users from the database.");
  }

  return users;
}
