#pragma once

#include <functional>
#include <vector>

#include "database/user.h"

namespace arm_face_id {

using domain::User;
using std::vector;

class Schema {
 public:
  using UpdateFunc = std::function<void(User&)>;

  virtual bool SaveUser(User& saved_user) = 0;
  virtual bool RemoveUser(int user_id) = 0;
  virtual bool UpdateUser(int user_id, UpdateFunc update_func) = 0;
  virtual User GetUserByID(int user_id) = 0;
  virtual vector<User> ListAllUsers() = 0;
};
}  // namespace arm_face_id