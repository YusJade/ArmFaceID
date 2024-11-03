cmake_minimum_required(VERSION 3.10.2)
project(arm_face_id LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Qt 信号/槽机制
set(CMAKE_AUTOMOC ON)

set(CMAKE_MODULE_PATH $ENV{SeetaFace6_DIR}/cmake ${SeetaFace6_DIR}/cmake)

set(SeetaFaceDetector_ROOT_DIR $ENV{SeetaFace6_DIR})
set(SeetaFaceLandmarker_ROOT_DIR $ENV{SeetaFace6_DIR})
set(SeetaFaceRecognizer_ROOT_DIR $ENV{SeetaFace6_DIR})

if(EXISTS "${CMAKE_SOURCE_DIR}/path_config.cmake")
  message(STATUS "应用配置文件 path_config.cmake")
  include(path_config.cmake)
endif()

find_package(Qt6 COMPONENTS Core Sql Widgets Multimedia MultimediaWidgets)
if(NOT ${Qt6_FOUND})
  find_package(Qt5 COMPONENTS Core Sql Widgets Multimedia MultimediaWidgets REQUIRED)
endif()
message(STATUS "Qt6 Found: ${Qt6_FOUND}")
message(STATUS "Qt5 Found: ${Qt5_FOUND}")
find_package(SeetaFaceRecognizer REQUIRED)
find_package(SeetaFaceDetector REQUIRED)
find_package(SeetaFaceLandmarker REQUIRED)
find_package(OpenCV REQUIRED)
find_package(spdlog)
find_package(absl)

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
add_subdirectory(server)
add_subdirectory(third_party)
# add_subdirectory(utils)
