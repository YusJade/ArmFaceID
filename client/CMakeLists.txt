cmake_minimum_required(VERSION 3.16)

project(ArmFaceID VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(protobuf_DIR D:\\cmake-install-modules\\lib\\cmake\\protobuf)
    set(absl_DIR D:\\cmake-install-modules\\lib\\cmake\\absl)
    set(utf8_range_DIR D:\\cmake-install-modules\\lib\\cmake\\utf8_range)
    set(SeetaFace_DIR "D:\\SeetaFace2\\lib\\cmake")
    set(CMAKE_PREFIX_PATH "E:/Qt/6.5.3/mingw_64")
    set(OpenCV_DIR D:\\cmake-install-modules\\opencv-4.10.0)
    set(gRPC_DIR D:\\cmake-install-modules\\lib\\cmake\\grpc)
elseif(LINUX)
    set(SeetaFace_DIR "/usr/local/lib/cmake")
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)
find_package(SeetaFace REQUIRED)
# find_package(protobuf CONFIG REQUIRED)
# find_package(gRPC CONFIG REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ArmFaceID
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        face_recognizer.h face_recognizer.cpp
        utils/utils.h utils/utils.cpp
        rpc_client.h rpc_client.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ArmFaceID APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ArmFaceID SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ArmFaceID
            ${PROJECT_SOURCES}
        )
    endif()
endif()

include_directories(${OpenCV_INCLUDE_DIRS})

message("链接到lib:")
message(${OpenCV_LIBS})

target_link_libraries(ArmFaceID PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${OpenCV_LIBS}
    face_proto)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ArmFaceID)
endif()
set_target_properties(ArmFaceID PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

message("GNUInstallDirs")
message(GNUInstallDirs)

include(GNUInstallDirs)
install(TARGETS ArmFaceID
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ArmFaceID)
endif()
