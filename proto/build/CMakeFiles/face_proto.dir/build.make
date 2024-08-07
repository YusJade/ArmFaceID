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
CMAKE_SOURCE_DIR = E:\Documents\CodeField\ArmFaceID\proto

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = E:\Documents\CodeField\ArmFaceID\proto\build

# Include any dependencies generated for this target.
include CMakeFiles/face_proto.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/face_proto.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/face_proto.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/face_proto.dir/flags.make

face_network.pb.h: E:/Documents/CodeField/ArmFaceID/proto/face_network.proto
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=E:\Documents\CodeField\ArmFaceID\proto\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Running cpp protocol buffer compiler on face_network.proto"
	D:\cmake-install-modules\bin\protoc.exe --cpp_out :E:/Documents/CodeField/ArmFaceID/proto/build -I E:/Documents/CodeField/ArmFaceID/proto E:/Documents/CodeField/ArmFaceID/proto/face_network.proto

face_network.pb.cc: face_network.pb.h
	@$(CMAKE_COMMAND) -E touch_nocreate face_network.pb.cc

face_network.grpc.pb.h: E:/Documents/CodeField/ArmFaceID/proto/face_network.proto
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=E:\Documents\CodeField\ArmFaceID\proto\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Running grpc protocol buffer compiler on face_network.proto"
	D:\cmake-install-modules\bin\protoc.exe --grpc_out :E:/Documents/CodeField/ArmFaceID/proto/build --plugin=protoc-gen-grpc=D:/cmake-install-modules/bin/grpc_cpp_plugin.exe -I E:/Documents/CodeField/ArmFaceID/proto E:/Documents/CodeField/ArmFaceID/proto/face_network.proto

face_network.grpc.pb.cc: face_network.grpc.pb.h
	@$(CMAKE_COMMAND) -E touch_nocreate face_network.grpc.pb.cc

CMakeFiles/face_proto.dir/face_network.pb.obj: CMakeFiles/face_proto.dir/flags.make
CMakeFiles/face_proto.dir/face_network.pb.obj: CMakeFiles/face_proto.dir/includes_CXX.rsp
CMakeFiles/face_proto.dir/face_network.pb.obj: face_network.pb.cc
CMakeFiles/face_proto.dir/face_network.pb.obj: CMakeFiles/face_proto.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\Documents\CodeField\ArmFaceID\proto\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/face_proto.dir/face_network.pb.obj"
	D:\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/face_proto.dir/face_network.pb.obj -MF CMakeFiles\face_proto.dir\face_network.pb.obj.d -o CMakeFiles\face_proto.dir\face_network.pb.obj -c E:\Documents\CodeField\ArmFaceID\proto\build\face_network.pb.cc

CMakeFiles/face_proto.dir/face_network.pb.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/face_proto.dir/face_network.pb.i"
	D:\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\Documents\CodeField\ArmFaceID\proto\build\face_network.pb.cc > CMakeFiles\face_proto.dir\face_network.pb.i

CMakeFiles/face_proto.dir/face_network.pb.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/face_proto.dir/face_network.pb.s"
	D:\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\Documents\CodeField\ArmFaceID\proto\build\face_network.pb.cc -o CMakeFiles\face_proto.dir\face_network.pb.s

CMakeFiles/face_proto.dir/face_network.grpc.pb.obj: CMakeFiles/face_proto.dir/flags.make
CMakeFiles/face_proto.dir/face_network.grpc.pb.obj: CMakeFiles/face_proto.dir/includes_CXX.rsp
CMakeFiles/face_proto.dir/face_network.grpc.pb.obj: face_network.grpc.pb.cc
CMakeFiles/face_proto.dir/face_network.grpc.pb.obj: CMakeFiles/face_proto.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\Documents\CodeField\ArmFaceID\proto\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/face_proto.dir/face_network.grpc.pb.obj"
	D:\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/face_proto.dir/face_network.grpc.pb.obj -MF CMakeFiles\face_proto.dir\face_network.grpc.pb.obj.d -o CMakeFiles\face_proto.dir\face_network.grpc.pb.obj -c E:\Documents\CodeField\ArmFaceID\proto\build\face_network.grpc.pb.cc

CMakeFiles/face_proto.dir/face_network.grpc.pb.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/face_proto.dir/face_network.grpc.pb.i"
	D:\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\Documents\CodeField\ArmFaceID\proto\build\face_network.grpc.pb.cc > CMakeFiles\face_proto.dir\face_network.grpc.pb.i

CMakeFiles/face_proto.dir/face_network.grpc.pb.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/face_proto.dir/face_network.grpc.pb.s"
	D:\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\Documents\CodeField\ArmFaceID\proto\build\face_network.grpc.pb.cc -o CMakeFiles\face_proto.dir\face_network.grpc.pb.s

# Object files for target face_proto
face_proto_OBJECTS = \
"CMakeFiles/face_proto.dir/face_network.pb.obj" \
"CMakeFiles/face_proto.dir/face_network.grpc.pb.obj"

# External object files for target face_proto
face_proto_EXTERNAL_OBJECTS =

libface_proto.a: CMakeFiles/face_proto.dir/face_network.pb.obj
libface_proto.a: CMakeFiles/face_proto.dir/face_network.grpc.pb.obj
libface_proto.a: CMakeFiles/face_proto.dir/build.make
libface_proto.a: CMakeFiles/face_proto.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=E:\Documents\CodeField\ArmFaceID\proto\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX static library libface_proto.a"
	$(CMAKE_COMMAND) -P CMakeFiles\face_proto.dir\cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\face_proto.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/face_proto.dir/build: libface_proto.a
.PHONY : CMakeFiles/face_proto.dir/build

CMakeFiles/face_proto.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\face_proto.dir\cmake_clean.cmake
.PHONY : CMakeFiles/face_proto.dir/clean

CMakeFiles/face_proto.dir/depend: face_network.grpc.pb.cc
CMakeFiles/face_proto.dir/depend: face_network.grpc.pb.h
CMakeFiles/face_proto.dir/depend: face_network.pb.cc
CMakeFiles/face_proto.dir/depend: face_network.pb.h
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" E:\Documents\CodeField\ArmFaceID\proto E:\Documents\CodeField\ArmFaceID\proto E:\Documents\CodeField\ArmFaceID\proto\build E:\Documents\CodeField\ArmFaceID\proto\build E:\Documents\CodeField\ArmFaceID\proto\build\CMakeFiles\face_proto.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/face_proto.dir/depend

