cmake_minimum_required(VERSION 3.10)
project(server)

if(WIN32)
    set(protobuf_DIR D:\\cmake-install-modules\\lib\\cmake\\protobuf)
    set(absl_DIR D:\\cmake-install-modules\\lib\\cmake\\absl)
    set(utf8_range_DIR D:\\cmake-install-modules\\lib\\cmake\\utf8_range)
    set(gRPC_DIR D:\\cmake-install-modules\\lib\\cmake\\grpc)
    set(OpenCV_DIR D:\\cmake-install-modules\\opencv-4.10.0)
endif()

# 查找 gRPC 包
find_package(protobuf CONFIG REQUIRED) # 使用 Modules 模式会找不到包
find_package(gRPC REQUIRED)
find_package(OpenCV REQUIRED)

# 设置源文件
set(SOURCES
    main.cpp
    rpc_manager.cpp 
)

# 创建可执行文件
add_executable(server ${SOURCES})
# 链接 gRPC 库
target_link_libraries(server
    PUBLIC
        face_proto
        ${OpenCV_LIBS}
)
target_include_directories(server PUBLIC
    ${gRPC_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${PROTOBUF_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_BINARY_DIR})
