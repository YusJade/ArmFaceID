# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.27

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = E:\Documents\CodeField\ArmFaceID\client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = E:\Documents\CodeField\ArmFaceID\client\build\Desktop_MinGW_64_bit-Debug

# Include any dependencies generated for this target.
include CMakeFiles/ArmFaceID.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ArmFaceID.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ArmFaceID.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ArmFaceID.dir/flags.make

CMakeFiles/ArmFaceID.dir/ArmFaceID_autogen/mocs_compilation.cpp.obj: CMakeFiles/ArmFaceID.dir/flags.make
CMakeFiles/ArmFaceID.dir/ArmFaceID_autogen/mocs_compilation.cpp.obj: CMakeFiles/ArmFaceID.dir/includes_CXX.rsp
CMakeFiles/ArmFaceID.dir/ArmFaceID_autogen/mocs_compilation.cpp.obj: ArmFaceID_autogen/mocs_compilation.cpp
CMakeFiles/ArmFaceID.dir/ArmFaceID_autogen/mocs_compilation.cpp.obj: CMakeFiles/ArmFaceID.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\Documents\CodeField\ArmFaceID\client\build\Desktop_MinGW_64_bit-Debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ArmFaceID.dir/ArmFaceID_autogen/mocs_compilation.cpp.obj"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ArmFaceID.dir/ArmFaceID_autogen/mocs_compilation.cpp.obj -MF CMakeFiles\ArmFaceID.dir\ArmFaceID_autogen\mocs_compilation.cpp.obj.d -o CMakeFiles\ArmFaceID.dir\ArmFaceID_autogen\mocs_compilation.cpp.obj -c E:\Documents\CodeField\ArmFaceID\client\build\Desktop_MinGW_64_bit-Debug\ArmFaceID_autogen\mocs_compilation.cpp

CMakeFiles/ArmFaceID.dir/ArmFaceID_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ArmFaceID.dir/ArmFaceID_autogen/mocs_compilation.cpp.i"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\Documents\CodeField\ArmFaceID\client\build\Desktop_MinGW_64_bit-Debug\ArmFaceID_autogen\mocs_compilation.cpp > CMakeFiles\ArmFaceID.dir\ArmFaceID_autogen\mocs_compilation.cpp.i

CMakeFiles/ArmFaceID.dir/ArmFaceID_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ArmFaceID.dir/ArmFaceID_autogen/mocs_compilation.cpp.s"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\Documents\CodeField\ArmFaceID\client\build\Desktop_MinGW_64_bit-Debug\ArmFaceID_autogen\mocs_compilation.cpp -o CMakeFiles\ArmFaceID.dir\ArmFaceID_autogen\mocs_compilation.cpp.s

CMakeFiles/ArmFaceID.dir/main.cpp.obj: CMakeFiles/ArmFaceID.dir/flags.make
CMakeFiles/ArmFaceID.dir/main.cpp.obj: CMakeFiles/ArmFaceID.dir/includes_CXX.rsp
CMakeFiles/ArmFaceID.dir/main.cpp.obj: E:/Documents/CodeField/ArmFaceID/client/main.cpp
CMakeFiles/ArmFaceID.dir/main.cpp.obj: CMakeFiles/ArmFaceID.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\Documents\CodeField\ArmFaceID\client\build\Desktop_MinGW_64_bit-Debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/ArmFaceID.dir/main.cpp.obj"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ArmFaceID.dir/main.cpp.obj -MF CMakeFiles\ArmFaceID.dir\main.cpp.obj.d -o CMakeFiles\ArmFaceID.dir\main.cpp.obj -c E:\Documents\CodeField\ArmFaceID\client\main.cpp

CMakeFiles/ArmFaceID.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ArmFaceID.dir/main.cpp.i"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\Documents\CodeField\ArmFaceID\client\main.cpp > CMakeFiles\ArmFaceID.dir\main.cpp.i

CMakeFiles/ArmFaceID.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ArmFaceID.dir/main.cpp.s"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\Documents\CodeField\ArmFaceID\client\main.cpp -o CMakeFiles\ArmFaceID.dir\main.cpp.s

CMakeFiles/ArmFaceID.dir/mainwindow.cpp.obj: CMakeFiles/ArmFaceID.dir/flags.make
CMakeFiles/ArmFaceID.dir/mainwindow.cpp.obj: CMakeFiles/ArmFaceID.dir/includes_CXX.rsp
CMakeFiles/ArmFaceID.dir/mainwindow.cpp.obj: E:/Documents/CodeField/ArmFaceID/client/mainwindow.cpp
CMakeFiles/ArmFaceID.dir/mainwindow.cpp.obj: CMakeFiles/ArmFaceID.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\Documents\CodeField\ArmFaceID\client\build\Desktop_MinGW_64_bit-Debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/ArmFaceID.dir/mainwindow.cpp.obj"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ArmFaceID.dir/mainwindow.cpp.obj -MF CMakeFiles\ArmFaceID.dir\mainwindow.cpp.obj.d -o CMakeFiles\ArmFaceID.dir\mainwindow.cpp.obj -c E:\Documents\CodeField\ArmFaceID\client\mainwindow.cpp

CMakeFiles/ArmFaceID.dir/mainwindow.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ArmFaceID.dir/mainwindow.cpp.i"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\Documents\CodeField\ArmFaceID\client\mainwindow.cpp > CMakeFiles\ArmFaceID.dir\mainwindow.cpp.i

CMakeFiles/ArmFaceID.dir/mainwindow.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ArmFaceID.dir/mainwindow.cpp.s"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\Documents\CodeField\ArmFaceID\client\mainwindow.cpp -o CMakeFiles\ArmFaceID.dir\mainwindow.cpp.s

CMakeFiles/ArmFaceID.dir/face_recognizer.cpp.obj: CMakeFiles/ArmFaceID.dir/flags.make
CMakeFiles/ArmFaceID.dir/face_recognizer.cpp.obj: CMakeFiles/ArmFaceID.dir/includes_CXX.rsp
CMakeFiles/ArmFaceID.dir/face_recognizer.cpp.obj: E:/Documents/CodeField/ArmFaceID/client/face_recognizer.cpp
CMakeFiles/ArmFaceID.dir/face_recognizer.cpp.obj: CMakeFiles/ArmFaceID.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\Documents\CodeField\ArmFaceID\client\build\Desktop_MinGW_64_bit-Debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/ArmFaceID.dir/face_recognizer.cpp.obj"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ArmFaceID.dir/face_recognizer.cpp.obj -MF CMakeFiles\ArmFaceID.dir\face_recognizer.cpp.obj.d -o CMakeFiles\ArmFaceID.dir\face_recognizer.cpp.obj -c E:\Documents\CodeField\ArmFaceID\client\face_recognizer.cpp

CMakeFiles/ArmFaceID.dir/face_recognizer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ArmFaceID.dir/face_recognizer.cpp.i"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\Documents\CodeField\ArmFaceID\client\face_recognizer.cpp > CMakeFiles\ArmFaceID.dir\face_recognizer.cpp.i

CMakeFiles/ArmFaceID.dir/face_recognizer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ArmFaceID.dir/face_recognizer.cpp.s"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\Documents\CodeField\ArmFaceID\client\face_recognizer.cpp -o CMakeFiles\ArmFaceID.dir\face_recognizer.cpp.s

CMakeFiles/ArmFaceID.dir/utils/utils.cpp.obj: CMakeFiles/ArmFaceID.dir/flags.make
CMakeFiles/ArmFaceID.dir/utils/utils.cpp.obj: CMakeFiles/ArmFaceID.dir/includes_CXX.rsp
CMakeFiles/ArmFaceID.dir/utils/utils.cpp.obj: E:/Documents/CodeField/ArmFaceID/client/utils/utils.cpp
CMakeFiles/ArmFaceID.dir/utils/utils.cpp.obj: CMakeFiles/ArmFaceID.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\Documents\CodeField\ArmFaceID\client\build\Desktop_MinGW_64_bit-Debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/ArmFaceID.dir/utils/utils.cpp.obj"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ArmFaceID.dir/utils/utils.cpp.obj -MF CMakeFiles\ArmFaceID.dir\utils\utils.cpp.obj.d -o CMakeFiles\ArmFaceID.dir\utils\utils.cpp.obj -c E:\Documents\CodeField\ArmFaceID\client\utils\utils.cpp

CMakeFiles/ArmFaceID.dir/utils/utils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ArmFaceID.dir/utils/utils.cpp.i"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\Documents\CodeField\ArmFaceID\client\utils\utils.cpp > CMakeFiles\ArmFaceID.dir\utils\utils.cpp.i

CMakeFiles/ArmFaceID.dir/utils/utils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ArmFaceID.dir/utils/utils.cpp.s"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\Documents\CodeField\ArmFaceID\client\utils\utils.cpp -o CMakeFiles\ArmFaceID.dir\utils\utils.cpp.s

# Object files for target ArmFaceID
ArmFaceID_OBJECTS = \
"CMakeFiles/ArmFaceID.dir/ArmFaceID_autogen/mocs_compilation.cpp.obj" \
"CMakeFiles/ArmFaceID.dir/main.cpp.obj" \
"CMakeFiles/ArmFaceID.dir/mainwindow.cpp.obj" \
"CMakeFiles/ArmFaceID.dir/face_recognizer.cpp.obj" \
"CMakeFiles/ArmFaceID.dir/utils/utils.cpp.obj"

# External object files for target ArmFaceID
ArmFaceID_EXTERNAL_OBJECTS =

ArmFaceID.exe: CMakeFiles/ArmFaceID.dir/ArmFaceID_autogen/mocs_compilation.cpp.obj
ArmFaceID.exe: CMakeFiles/ArmFaceID.dir/main.cpp.obj
ArmFaceID.exe: CMakeFiles/ArmFaceID.dir/mainwindow.cpp.obj
ArmFaceID.exe: CMakeFiles/ArmFaceID.dir/face_recognizer.cpp.obj
ArmFaceID.exe: CMakeFiles/ArmFaceID.dir/utils/utils.cpp.obj
ArmFaceID.exe: CMakeFiles/ArmFaceID.dir/build.make
ArmFaceID.exe: E:/Qt/6.5.3/mingw_64/lib/libQt6Widgets.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_gapi4100.dll.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_highgui4100.dll.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_ml4100.dll.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_stitching4100.dll.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_video4100.dll.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_videoio4100.dll.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_face4100.dll.a
ArmFaceID.exe: E:/Qt/6.5.3/mingw_64/lib/libQt6Gui.a
ArmFaceID.exe: E:/Qt/6.5.3/mingw_64/lib/libQt6Core.a
ArmFaceID.exe: E:/Qt/6.5.3/mingw_64/lib/libQt6EntryPoint.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_imgcodecs4100.dll.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_objdetect4100.dll.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_calib3d4100.dll.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_dnn4100.dll.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_features2d4100.dll.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_flann4100.dll.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_photo4100.dll.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_imgproc4100.dll.a
ArmFaceID.exe: D:/opencv-5.10.0/x64/mingw/lib/libopencv_core4100.dll.a
ArmFaceID.exe: CMakeFiles/ArmFaceID.dir/linkLibs.rsp
ArmFaceID.exe: CMakeFiles/ArmFaceID.dir/objects1.rsp
ArmFaceID.exe: CMakeFiles/ArmFaceID.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=E:\Documents\CodeField\ArmFaceID\client\build\Desktop_MinGW_64_bit-Debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable ArmFaceID.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\ArmFaceID.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ArmFaceID.dir/build: ArmFaceID.exe
.PHONY : CMakeFiles/ArmFaceID.dir/build

CMakeFiles/ArmFaceID.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\ArmFaceID.dir\cmake_clean.cmake
.PHONY : CMakeFiles/ArmFaceID.dir/clean

CMakeFiles/ArmFaceID.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" E:\Documents\CodeField\ArmFaceID\client E:\Documents\CodeField\ArmFaceID\client E:\Documents\CodeField\ArmFaceID\client\build\Desktop_MinGW_64_bit-Debug E:\Documents\CodeField\ArmFaceID\client\build\Desktop_MinGW_64_bit-Debug E:\Documents\CodeField\ArmFaceID\client\build\Desktop_MinGW_64_bit-Debug\CMakeFiles\ArmFaceID.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/ArmFaceID.dir/depend

