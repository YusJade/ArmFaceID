# 设置交叉编译器的位置
set(CMAKE_C_COMPILER "/opt/gcc-aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "/opt/gcc-aarch64-linux-gnu/bin/aarch64-linux-gnu-g++")

set(CMAKE_IGNORE_PATH /usr/include/ /usr/local/include/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
# 指定目标系统的类型
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(Qt5_DIR "/home/yu/aarch64/lib/cmake/grpc/")

set(x264_LIBS /home/yu/aarch64/x264/lib/libx264.so)

set(FFMPEG_LIBS
    /home/yu/aarch64/ffmpeg/lib/libavcodec.so
    /home/yu/aarch64/ffmpeg/lib/libpostproc.so
    /home/yu/aarch64/ffmpeg/lib/libswresample.so
    /home/yu/aarch64/ffmpeg/lib/libswscale.so
    /home/yu/aarch64/ffmpeg/lib/libavutil.so
    /home/yu/aarch64/ffmpeg/lib/libavdevice.so
    /home/yu/aarch64/ffmpeg/lib/libavfilter.so
    /home/yu/aarch64/ffmpeg/lib/libavformat.so)

set(CMAKE_FIND_ROOT_PATH
    /home/yu/aarch64/grpc/lib/cmake/grpc/
    /home/yu/aarch64/grpc/lib/cmake/absl/
    /home/yu/aarch64/grpc/lib/cmake/protobuf/
    /home/yu/aarch64/grpc/lib/cmake/re2/
    /home/yu/aarch64/grpc/lib/cmake/c-ares/
    /home/yu/aarch64/grpc/lib/cmake/utf8_range/
    /home/yu/aarch64/ffmpeg
    /home/yu/aarch64/opencv/lib/cmake/opencv4/
    /home/yu/aarch64/qt5.12.10-arm/lib/cmake/Qt5
    /home/yu/aarch64/SeetaFace2/lib/cmake/
    /home/yu/aarch64/fmt/lib/cmake/fmt/)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
