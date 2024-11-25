#pragma once

#include <qsqldatabase.h>
#include <qsqlquery.h>

#include <string>

#include "database/schema.h"

namespace arm_face_id {
using std::string;
class MySqlSchema : public Schema {
 public:
  MySqlSchema(string hostname, int port, string db, string user_name,
              string password);

  bool SaveUser(const User &saved_user) override;
  bool UpdateUser(int user_id, UpdateFunc update_func) override;
  User GetUserByID(int user_id) override;
  bool RemoveUser(int user_id) override;
  vector<User> ListAllUsers() override;

 private:
  QSqlDatabase db_;
  QSqlQuery query_;
};
}  // namespace arm_face_id