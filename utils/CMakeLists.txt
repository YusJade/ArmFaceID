# 定义 proto 文件
set(UTILS_FILES function.cpp interface.cpp face_camera.cpp)

if(${Qt6_FOUND})
  set(Qt_LIBS Qt6::Core Qt6::Widgets)
else()
  set(Qt_LIBS Qt5::Core Qt5::Widgets)
endif()

set(HEAD_FILES ./function.h ./interface.h ./face_camera.h)

# 定义一个自定义目标，每次构建时复制头文件
add_custom_target(copy_headers ALL
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${HEAD_FILES} ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${HEAD_FILES}
)


add_library(utils ${UTILS_FILES})
target_link_libraries(utils PUBLIC ${OpenCV_LIBS} ${Qt_LIBS} spdlog::spdlog)
target_include_directories(
  utils PUBLIC ${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
