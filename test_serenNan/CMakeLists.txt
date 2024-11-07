cmake_minimum_required(VERSION 3.10)

project(my_grpc_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 添加 gRPC 和 Protocol Buffers 的库
find_package(protobuf REQUIRED)
find_package(gRPC REQUIRED)

# 添加生成的代码
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/generated)

# 添加子目录
add_subdirectory(src/server)
add_subdirectory(src/client)
