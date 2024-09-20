# 设置交叉编译器的位置
set(CMAKE_C_COMPILER "/opt/gcc-aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "/opt/gcc-aarch64-linux-gnu/bin/aarch64-linux-gnu-g++")

# 指定目标系统的类型
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(Qt5_DIR "/home/yu/aarch64/lib/cmake/grpc/")

set(CMAKE_FIND_ROOT_PATH 
 /home/yu/aarch64/lib/cmake/absl/
 /home/yu/aarch64/lib/cmake/protobuf/
 /home/yu/aarch64/ffmpeg
 /home/yu/aarch64/opencv/lib/cmake/opencv4/
 /home/yu/aarch64/qt5.12.10-arm/lib/cmake/Qt5
 /home/yu/aarch64/SeetaFace2/lib/cmake/
 /home/yu/aarch64/fmt/lib/cmake/fmt/)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
