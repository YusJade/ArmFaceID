#pragma once

#include <qglobal.h>
#include <qimage.h>
#include <qstringview.h>
#include <qtypes.h>

#include <string>
#include <vector>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>

namespace arm_face_id {

namespace data {

struct User {
  int id = -1;
  std::string nick_name;
  std::string email;
  QImage profile_pic;
  QImage face_img;
  QVector<float> face_feature;

  // QVector<qint8> profile_pic_bytes;
  // QVector<qint8> face_img_bytes;
  // std::vector<uint8_t> profile_pic_bytes;
  // std::vector<uint8_t> face_img_bytes;
  // std::vector<float> face_feature;

  User() = default;

  User(int id_, std::string nick_name_, std::string email_,
       const QImage& profile_pic_, const QImage& face_img_,
       const std::vector<float>& face_feature_);

  User(int id_, std::string nick_name_, std::string email_,
       const QImage& profile_pic_, const QImage& face_img_,
       const QVector<float>& face_feature_);
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
  FaceDataBase(FaceDataBase&& other) = delete;

 public:
  bool InitDb();
  int AddUser(std::string nick_name, std::string email, QImage face_img);
  int AddUser(const data::User&);
  int GetUserById(int id, User& res);
  bool RemoveUser(int user_id);

  void LoadToCache();

  User GetUserById(int id);
  const std::vector<User>& Users() const { return users_; }

  std::vector<std::vector<float>> features_vec_;

 private:
  QSqlDatabase db_;
  QSqlQuery sql_query_;

  std::vector<User> users_;
  static FaceDataBase* instance_;
};
}  // namespace data
}  // namespace arm_face_id

// 重载 << 运算符用于序列化 std::vector<float>
QDataStream& operator<<(QDataStream& out, const std::vector<float>& vec);

// 重载 >> 运算符用于反序列化 std::vector<float>
QDataStream& operator>>(QDataStream& in, std::vector<float>& vec);