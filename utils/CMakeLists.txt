# 定义 proto 文件
set(UTILS_FILES function.cpp interface.cpp)

if(${Qt6_FOUND})
  set(Qt_LIBS Qt6::Core Qt6::Widgets)
else()
  set(Qt_LIBS Qt5::Core Qt5::Widgets)
endif()

set(HEAD_FILES ./function.h ./interface.h)
file(COPY ${HEAD_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_library(utils ${UTILS_FILES})
target_link_libraries(utils PUBLIC ${OpenCV_LIBS} ${Qt_LIBS})
target_include_directories(
  utils PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
               ${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
