cmake_minimum_required(VERSION 3.16)
project(client)

if(WIN32)
  set(Qt6_DIR E:/Qt/6.5.3/mingw_64/lib/cmake)
endif()

add_executable(client
  main.cpp
  utils.cpp
  rpc_client.cpp
  face_processor.cpp
  face_processor_listener.cpp
  display_widget.cpp)

find_package(OpenCV REQUIRED)
find_package(gflags REQUIRED)
find_package(absl REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent)

target_include_directories(client 
  PUBLIC
    ${GFLAGS_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS})

target_link_libraries(client  
  PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    ${OpenCV_LIBS}
    absl::flags
    absl::log
    absl::flags_parse
    absl::flags_usage
    # ${GFLAGS_LIBRARIES}
    face_proto)
