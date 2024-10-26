cmake_minimum_required(VERSION 3.10.2)
project(arm_face_id LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Qt 信号/槽机制
set(CMAKE_AUTOMOC ON)

option(USE_ELA_GUI "Set to On to use ElaWidgetTools" ON)

# Define a function to find and check a package
function(find_and_check_package package_name)
  # Attempt to find the package
  find_package(${package_name} REQUIRED)

  # Check if the package was found
  if(${package_name}_FOUND)
    message(STATUS "${package_name} package found.")
    # Optionally, output more details get_target_property(target_location
    # ${package_name}::${package_name} LOCATION) if (target_location)
    # message(STATUS "${package_name} target location: ${target_location}")
    # else() message(STATUS "${package_name} target location not found.")
    # endif()
  else()
    message(FATAL_ERROR "${package_name} package not found.")
  endif()
endfunction()

if(WIN32)
  message(STATUS "Configuring for Windows")
  set(gflags_DIR D:/cmake-install-modules/gflags/lib/cmake/gflags)
  set(utf8_range_DIR D:/cmake-install-modules/lib/cmake/utf8_range)
  set(gRPC_DIR D:/cmake-install-modules/lib/cmake/grpc)
  set(OpenCV_DIR D:/cmake-install-modules/opencv-4.10.0)
  set(SeetaFace_DIR D:/cmake-install-modules/SeetaFace/lib/cmake)
  set(protobuf_DIR D:/cmake-install-modules/lib/cmake/protobuf)
  set(absl_DIR D:/cmake-install-modules/lib/cmake/absl)
  set(spdlog_DIR D:/cmake-install-modules/spdlog/lib/cmake/spdlog)
  set(fmt_DIR D:/cmake-install-modules/fmt/lib/cmake/fmt/)
  set(c-ares_DIR D:/cmake-install-modules/lib/cmake/c-ares)
  set(SeetaFac_INCLUDE_DIRS D:/cmake-install-modules/SeetaFace/include)
  set(fmt_INCLUDE_DIRS D:/cmake-install-modules/spdlog/include/spdlog)

  set(ElaWidgets_LIBS
      E:/Documents/CodeField/ElaWidgetTools/build/libelawidgettools.a)
  set(ElaWidgets_INCLUDE_DIRS E:/Documents/CodeField/ElaWidgetTools/src/include)

  find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent Sql)
elseif(UNIX AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  message(STATUS "Configuring for AMD64 Linux")
  set(CMAKE_PREFIX_PATH /home/yu/Qt/6.5.3/gcc_64/lib/cmake/)
  set(SeetaFace_DIR /usr/local/lib/cmake/)
  set(Qt6_DIR /home/yu/Qt/6.5.3/gcc_64/lib/cmake/Qt6/)
  set(gRPC_DIR /home/yu/aarch64/lib/cmake/grpc/)
  set(protobuf_DIR /home/yu/aarch64/lib/cmake/protobuf/)

  find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent Sql)

  set(CMAKE_MODULE_PATH /home/yu/codefield/SeetaFace6/build/cmake/)
  find_package(SeetaFaceRecognizer REQUIRED)
  find_package(SeetaFaceDetector REQUIRED)
  find_package(SeetaFaceLandmarker REQUIRED)

  # TO FIX CMAKE ERROR
  set(SeetaFaceLandmarker_INCLUDE_DIR)

  set(ElaWidgets_LIBS /home/yu/amd64/elawidget/lib/libelawidgettools.so)
  set(ElaWidgets_INCLUDE_DIRS /home/yu/amd64/elawidget/include/)
  if(${Qt6_FOUND})
    message(STATUS "Qt6 Found :>")
  else()
    message(STATUS "Qt6 Not Found :<")
  endif()
elseif(UNIX AND CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
  message(STATUS "Configuring for AArch64 (ARM64) Linux")

  message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
  message(STATUS "CMAKE_FIND_ROOT_PATH: ${CMAKE_FIND_ROOT_PATH}")
  # find_package(protobuf REQUIRED) find_package(gRPC REQUIRED)
  find_package(Qt5 COMPONENTS Core Widgets Concurrent Sql)
  # find_package(SeetaFace) find_package(spdlog) find_package(fmt)
endif()

find_package(protobuf CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(c-ares CONFIG REQUIRED)
# find_package(Qt5 COMPONENTS Core Widgets)
find_package(OpenCV CONFIG REQUIRED)
find_package(SeetaFace)
find_and_check_package(spdlog)
find_package(fmt)
# find_package(gflags REQUIRED) 查找 gRPC 包 find_package(OpenCV REQUIRED)
# find_package(SeetaFace CONFIG REQUIRED)

# find_package(absl REQUIRED) find_package(spdlog REQUIRED) find_package(fmt
# CONFIG REQUIRED)

# add_subdirectory(proto)
add_subdirectory(server)
# add_subdirectory(client)
# add_subdirectory(model)
add_subdirectory(utils)
