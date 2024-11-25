#pragma once

#include <qimage.h>

#include <string>

namespace arm_face_id {
namespace domain {
using std::string;

struct User {
  string user_id;
  string user_name;
  string email;
  QImage profile_pic;
  QImage face_img;
};
}  // namespace domain
}  // namespace arm_face_id