cmake_minimum_required(VERSION 3.16)
project(client)

# if(WIN32) set(Qt6_DIR E:/Qt/6.5.3/mingw_64/lib/cmake) endif()

if(CMAKE_CROSSCOMPILING)
  set(Qt_LIBS Qt5::Core Qt5::Widgets Qt5::Concurrent)
else()
  set(Qt_LIBS Qt6::Core Qt6::Widgets Qt6::Concurrent)
endif()

if(NOT CMAKE_CROSSCOMPILING)
  set(FFMPEG_LIBS "")
  set(x264_LIBS "")
endif()

add_executable(
  client
  main.cpp
  utils.cpp
  rpc_client.cpp
  face_processor.cpp
  face_processor_listener.cpp
  gui.cpp
  camera.cpp)

add_dependencies(client utils_headers)

# find_package(OpenCV REQUIRED)

# find_package(absl REQUIRED) find_package(Qt6 REQUIRED COMPONENTS Core Widgets
# Concurrent)

target_include_directories(
  client PUBLIC # ${GFLAGS_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
                ${OpenCV_INCLUDE_DIRS})

target_link_libraries(
  client
  PRIVATE ${Qt_LIBS}
          ${OpenCV_LIBS}
          ${FFMPEG_LIBS}
          ${x264_LIBS}
          utils
          spdlog::spdlog
          absl::flags
          absl::log
          absl::flags_parse
          absl::flags_usage
          # ${GFLAGS_LIBRARIES}
          face_proto)
