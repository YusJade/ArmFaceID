# 设置源文件
set(SOURCES 
main.cpp 
rpc_manager.cpp 
face_database.cpp 
engine.cpp 

gui/gui.cpp 
gui/ela_gui.cpp 
gui/register_page.cpp 
gui/recognition_page.cpp 
gui/tableview_page.cpp 
table_model/user_table_model.cpp)

set(QT_LIBS Qt6::Core Qt6::Widgets Qt6::Concurrent Qt6::Sql)
if(${Qt5_FOUND})
  set(QT_LIBS Qt5::Core Qt5::Widgets Qt5::Concurrent Qt5::Sql)
endif()

# 创建可执行文件
add_executable(server ${SOURCES})

add_dependencies(server utils)

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
