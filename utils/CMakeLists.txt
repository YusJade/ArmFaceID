# 定义 proto 文件
set(UTILS_FILES function.cpp interface.cpp)

if(${Qt6_FOUND})
  set(Qt_LIBS Qt6::Core Qt6::Widgets)
else()
  set(Qt_LIBS Qt5::Core Qt5::Widgets)
endif()

add_library(utils ${UTILS_FILES})
target_link_libraries(utils PUBLIC ${OpenCV_LIBS} ${Qt_LIBS})
target_include_directories(utils PUBLIC ${OpenCV_INCLUDE_DIRS}
                                        ${CMAKE_CURRENT_BINARY_DIR})
