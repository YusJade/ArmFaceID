cmake_minimum_required(VERSION 3.16)
project(client)

if(WIN32)
  set(gflags_DIR D:/cmake-install-modules/gflags/lib/cmake/gflags)
endif()

add_executable(client
  main.cpp)

find_package(gflags REQUIRED)

target_include_directories(client 
  PUBLIC
    ${GFLAGS_INCLUDE_DIR})

target_link_libraries(client 
  PUBLIC
    ${GFLAGS_LIBRARIES})
