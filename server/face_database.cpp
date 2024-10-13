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
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include <QtSql/QSqlError>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include "function.h"

using namespace arm_face_id::data;

constexpr const char* db_driver = "QSQLITE";
constexpr const char* db_name = "db_arm_face_id.db";

constexpr const char* sql_create_tb_usr =
    "CREATE TABLE tb_user(user_id INTEGER PRIMARY KEY AUTOINCREMENT, "
    "nick_name VARCHAR, "
    "face_img BLOB,"
    "email VARCHAR,"
    "profile_pic BLOB,"
    "face_feature BLOB);";

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

// TODO: 序列化问题
int FaceDataBase::AddUser(const User& user) {
  QByteArray face_img_bytes;
  QDataStream face_img_stream(&face_img_bytes, QIODeviceBase::WriteOnly);
  // std::copy(user.face_img_bytes.begin(), user.face_img_bytes.end(),
  //           face_img_bytes.begin());
  QByteArray profile_pic_bytes;
  QDataStream profile_pic_stream(&profile_pic_bytes, QIODeviceBase::WriteOnly);
  // std::copy(user.profile_pic_bytes.begin(), user.profile_pic_bytes.end(),
  //           profile_pic_bytes.begin());
  QByteArray face_feature_bytes;
  QDataStream vec_stream(&face_feature_bytes, QIODeviceBase::WriteOnly);
  vec_stream << user.face_feature;
  // buffer.open(QIODevice::WriteOnly);
  // face_img.save(&buffer, "JPEG");
  // buffer.close();

  DBConnection db_conn(db_driver, db_name);
  auto& sql_query = db_conn.GetSqlQuery();
  sql_query.prepare(
      "INSERT INTO tb_user(user_id, nick_name, email, profile_pic, "
      "face_img, face_feature) VALUES (NULL, :nick_name, :email, :profile_pic, "
      ":face_img, :face_feature);");
  // sql_query.prepare(
  //     "INSERT INTO tb_user(user_id, nick_name, email, "
  //     "face_img) VALUES (NULL, 'test', 'test', NULL)");

  sql_query.bindValue(":nick_name", QString::fromStdString(user.nick_name));
  sql_query.bindValue(":email", QString::fromStdString(user.email));
  sql_query.bindValue(":profile_pic", profile_pic_bytes);
  sql_query.bindValue(":face_img", face_img_bytes);
  sql_query.bindValue(":face_feature", face_feature_bytes);

  if (sql_query.exec()) {
    spdlog::info("已向数据库插入用户数据 ~ :  {} ({},{})",
                 sql_query.lastError().databaseText(), user.nick_name,
                 user.email);
    return sql_query.lastInsertId().toInt();
  }
  spdlog::error("无法向数据库插入用户数据！: {} ({},{})",
                sql_query.lastError().text() +
                    sql_query.lastError().driverText() +
                    sql_query.lastError().databaseText(),
                user.nick_name, user.email);

  return -1;
}

int FaceDataBase::GetUserById(int id, User& res) {
  DBConnection db_conn(db_driver, db_name);
  QSqlQuery& sql_query = db_conn.GetSqlQuery();
  sql_query.prepare(
      "SELECT user_id, email, nick_name, face_img FROM tb_user WHERE user_id = "
      "?");
  sql_query.bindValue(0, id);

  if (!sql_query.exec(sql_select_tb_usr)) {
    spdlog::error("无法读取数据库！>_< : {}",
                  sql_query.lastError().text().toStdString());
    return -1;
  }

  sql_query.next();

  int user_id = sql_query.value(0).toInt();
  std::string nick_name = sql_query.value(1).toString().toStdString();
  std::string email = sql_query.value(2).toString().toStdString();

  QByteArray profile_pic_qbytes = sql_query.value(3).toByteArray();
  QImage profile_pic;
  profile_pic.loadFromData(profile_pic_qbytes);

  QByteArray face_img_qbytes = sql_query.value(3).toByteArray();
  QImage face_img;
  face_img.loadFromData(face_img_qbytes);

  QByteArray feature_qbytes = sql_query.value(5).toByteArray();
  QDataStream in_feature(feature_qbytes);
  QVector<float> face_feature;
  in_feature >> face_feature;

  // User user;
  res.id = user_id;
  res.email = email;
  res.nick_name = nick_name;
  res.profile_pic = profile_pic;
  res.face_img = face_img;
  res.face_feature = face_feature;

  return res.id;
}

bool FaceDataBase::RemoveUser(int user_id) { return 0; }

void FaceDataBase::LoadToCache() {
  auto db_conn = DBConnection(db_driver, db_name);
  auto& sql_query = db_conn.GetSqlQuery();
  if (!sql_query.exec("SELECT user_id, nick_name, email, profile_pic, "
                      "face_img, face_feature FROM tb_user;")) {
    spdlog::error("无法读取数据库！>_< : {}",
                  sql_query.lastError().text().toStdString());
    return;
  }

  users_.clear();
  while (sql_query.next()) {
    int user_id = sql_query.value(0).toInt();
    std::string nick_name = sql_query.value(1).toString().toStdString();
    std::string email = sql_query.value(2).toString().toStdString();

    QByteArray profile_pic_qbytes = sql_query.value(3).toByteArray();
    QImage profile_pic;
    profile_pic.loadFromData(profile_pic_qbytes);

    QByteArray face_img_qbytes = sql_query.value(3).toByteArray();
    QImage face_img;
    face_img.loadFromData(face_img_qbytes);

    QByteArray feature_qbytes = sql_query.value(5).toByteArray();
    QDataStream in_feature(feature_qbytes);
    QVector<float> face_feature;
    in_feature >> face_feature;
    // in >> face_feature;

    User user;
    user.id = user_id;
    user.email = email;
    user.nick_name = nick_name;
    user.profile_pic = profile_pic;
    user.face_img = face_img;
    user.face_feature = face_feature;
    // user.profile_pic_bytes = profile_pic_bytes;
    // user.face_img_bytes = face_img_bytes;
    // user.face_feature = face_feature;

    users_.push_back(user);
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

User::User(int id_, std::string nick_name_, std::string email_,
           const QImage& profile_pic_, const QImage& face_img_,
           const std::vector<float>& face_feature_)
    : id(id_),
      nick_name(nick_name_),
      email(email_),
      profile_pic(profile_pic_),
      face_img(face_img_),
      face_feature(face_feature_.begin(), face_feature_.end()) {}

User::User(int id_, std::string nick_name_, std::string email_,
           const QImage& profile_pic_, const QImage& face_img_,
           const QVector<float>& face_feature_)
    : id(id_),
      nick_name(nick_name_),
      email(email_),
      profile_pic(profile_pic_),
      face_img(face_img_),
      face_feature(face_feature_) {}

// 重载 << 运算符用于序列化 std::vector<float>
QDataStream& operator<<(QDataStream& out, const std::vector<float>& vec) {
  out << static_cast<quint32>(vec.size());  // 写入 vector 的大小
  for (float value : vec) {
    out << value;  // 写入每个 float 值
  }
  return out;
}

// 重载 >> 运算符用于反序列化 std::vector<float>
QDataStream& operator>>(QDataStream& in, std::vector<float>& vec) {
  quint32 size;
  in >> size;        // 读取 vector 的大小
  vec.resize(size);  // 调整 vector 的大小
  for (quint32 i = 0; i < size; ++i) {
    in >> vec[i];  // 读取每个 float 值
  }
  return in;
}