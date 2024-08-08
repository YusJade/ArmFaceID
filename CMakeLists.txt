cmake_minimum_required(VERSION 3.10.2)
project(arm_face_id LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE DEBUG)

add_subdirectory(proto)
add_subdirectory(server)
add_subdirectory(client)