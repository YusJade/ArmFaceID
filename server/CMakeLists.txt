set(SOURCES
    src/main.cpp
    src/face_database.cpp
    src/engine.cpp
    src/camera.cpp
    src/gui.cpp
    src/utils/base.cpp
    )

if(${Qt6_FOUND})
set(Qt_LIBS Qt6::Core Qt6::Widgets Qt6::Sql Qt6::Multimedia Qt6::MultimediaWidgets)
elseif(${Qt5_FOUND})
  set(Qt_LIBS Qt5::Core Qt5::Widgets Qt5::Sql Qt5::Multimedia Qt5::MultimediaWidgets)
endif()

add_executable(server ${SOURCES})

# add_dependencies(server utils_headers)

target_link_libraries(
  server
  PUBLIC spdlog::spdlog
         rpc
         ${Qt_LIBS}
         absl::flags
         absl::log
         absl::flags_parse
         absl::flags_usage
         ${OpenCV_LIBS}
         ${SeetaFaceDetector_LIBRARY}
         ${SeetaFaceRecognizer_LIBRARY}
         ${SeetaFaceLandmarker_LIBRARY})

target_include_directories(
  server
  PUBLIC 
         include
         ${CMAKE_SOURCE_DIR}/third_party
         ${CMAKE_SOURCE_DIR}/third_party/treasure_chest
         ${OpenCV_INCLUDE_DIRS}
         ${SeetaFaceDetector_INCLUDE_DIR}
         ${SeetaFaceRecognizer_INCLUDE_DIR}
         ${SeetaFaceLandmarker_INCLUDE_DIR}
         ${ElaWidgets_INCLUDE_DIRS})
