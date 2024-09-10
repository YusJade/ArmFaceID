cmake_minimum_required(VERSION 3.10)
project(server)

if(WIN32)
    set(protobuf_DIR D:\\cmake-install-modules\\lib\\cmake\\protobuf)
    set(absl_DIR D:\\cmake-install-modules\\lib\\cmake\\absl)
    set(utf8_range_DIR D:\\cmake-install-modules\\lib\\cmake\\utf8_range)
    set(gRPC_DIR D:\\cmake-install-modules\\lib\\cmake\\grpc)
    set(OpenCV_DIR D:\\cmake-install-modules\\opencv-4.10.0)
    set(SeetaFace_DIR D:\\cmake-install-modules\\SeetaFace\\lib\\cmake)
elseif(UNIX)
    set(SeetaFace_DIR /usr/local/lib/cmake/)
    set(Qt6_DIR /home/yu/Qt/6.5.3/gcc_64/lib/cmake/Qt6/)
endif()

# 查找 gRPC 包
find_package(protobuf CONFIG REQUIRED) # 使用 Modules 模式会找不到包
find_package(gRPC REQUIRED)
find_package(OpenCV REQUIRED)
find_package(SeetaFace CONFIG REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent)
find_package(absl REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt)

# 设置源文件
set(SOURCES
    main.cpp
    rpc_manager.cpp 
    utils/utils.cpp
    gui.cpp
    engine.cpp
)

# 创建可执行文件
add_executable(server ${SOURCES})
# 链接 gRPC 库
target_link_libraries(server
    PUBLIC
        Qt6::Core
        Qt6::Widgets
        Qt6::Concurrent
        absl::flags
        absl::log
        absl::flags_parse
        absl::flags_usage
	fmt::fmt
        face_proto
        ${OpenCV_LIBS}
        ${SeetaFace_LIBRARIES}
)

target_include_directories(server PUBLIC
    # ${gRPC_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${SeetaFace_INCLUDE_DIRS}
    # ${PROTOBUF_INCLUDE_DIRS} 
    CMAKE_CURRENT_SOURCE_DIR
    )
