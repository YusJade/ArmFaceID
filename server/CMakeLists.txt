# if(WIN32) set(protobuf_DIR D:\\cmake-install-modules\\lib\\cmake\\protobuf)
# set(absl_DIR D:\\cmake-install-modules\\lib\\cmake\\absl) set(utf8_range_DIR
# D:\\cmake-install-modules\\lib\\cmake\\utf8_range) set(gRPC_DIR
# D:\\cmake-install-modules\\lib\\cmake\\grpc) set(OpenCV_DIR
# D:\\cmake-install-modules\\opencv-4.10.0) set(SeetaFace_DIR
# D:\\cmake-install-modules\\SeetaFace\\lib\\cmake) set(spdlog_DIR
# D:/cmake-install-modules/spdlog/lib/cmake/spdlog) set(fmt_DIR
# D:/cmake-install-modules/fmt/lib/cmake/fmt/) elseif(UNIX) set(SeetaFace_DIR
# /usr/local/lib/cmake/) set(Qt6_DIR /home/yu/Qt/6.5.3/gcc_64/lib/cmake/Qt6/)
# endif()

# find_package(protobuf CONFIG REQUIRED) # 使用 Modules 模式会找不到包 find_package(gRPC
# REQUIRED) find_package(OpenCV REQUIRED) find_package(SeetaFace CONFIG
# REQUIRED)

# set(Qt5_DIR "/home/yu/aarch64/lib/cmake/grpc/") find_package(Qt6 REQUIRED
# COMPONENTS Core QUIET Widgets QUIET Concurrent QUIET) find_package(Qt5
# REQUIRED COMPONENTS Core Widgets Concurrent QUIET)

# find_package(absl REQUIRED) find_package(spdlog REQUIRED) find_package(fmt
# CONFIG REQUIRED)

# 设置源文件
set(SOURCES main.cpp rpc_manager.cpp utils/utils.cpp gui.cpp engine.cpp ela_gui.cpp)

set(QT_LIBS Qt6::Core Qt6::Widgets Qt6::Concurrent)
if(${Qt5_FOUND})
  set(QT_LIBS Qt5::Core Qt5::Widgets Qt5::Concurrent)
endif()

# 创建可执行文件
add_executable(server ${SOURCES})


if(USE_ELA_GUI)
target_compile_definitions(server PUBLIC ELA_GUI)
endif()

# 链接 gRPC 库
target_link_libraries(
  server
  PUBLIC spdlog::spdlog
         ${QT_LIBS}
         absl::flags
         absl::log
         absl::flags_parse
         absl::flags_usage
         face_proto
         utils
         ${OpenCV_LIBS}
         ${SeetaFace_LIBRARIES}
         ${ElaWidgets_LIBS})

target_include_directories(
  server
  PUBLIC # ${gRPC_INCLUDE_DIRS}
         ${OpenCV_INCLUDE_DIRS}
         ${SeetaFace_INCLUDE_DIRS}
         # ${PROTOBUF_INCLUDE_DIRS}
         ${CMAKE_CURRENT_SOURCE_DIR}
         ${fmt_INCLUDE_DIRS}
         ${ElaWidgets_INCLUDE_DIRS})
