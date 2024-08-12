#include <iostream>

#include <gflags/gflags.h>

DEFINE_string(cascade_path, ".", "set the path of cascade`s path.");
DEFINE_string(camera_id, "0", "set the id of camera device.");

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, false);
  std::cout << FLAGS_camera_id;
  std::cout << FLAGS_cascade_path;
  return 0;
}