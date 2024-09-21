# 定义 proto 文件
set(PROTO_FILES face_network.proto)

# 设置 proto 文件路径
set(PROTO_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROTO_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

# 查找 Protobuf 编译器
find_program(Protobuf_PROTOC_EXECUTABLE protoc)
find_program(gRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)

if(NOT Protobuf_PROTOC_EXECUTABLE)
  message(
    FATAL_ERROR
      "Could not find protoc executable. Make sure it's installed and in your PATH."
  )
endif()

message(STATUS "Path of gRPC: ${gRPC_DIR} ${protobuf_DIR} ${absl_DIR}")
# 生成 Protobuf 和 gRPC 文件
foreach(proto_file ${PROTO_FILES})
  get_filename_component(proto_file_name ${proto_file} NAME_WE)

  # 生成命令
  add_custom_command(
    OUTPUT ${PROTO_BINARY_DIR}/${proto_file_name}.pb.cc
           ${PROTO_BINARY_DIR}/${proto_file_name}.grpc.pb.cc
    COMMAND
      /home/yu/aarch64/bin/protoc ARGS --proto_path=${PROTO_SOURCE_DIR}
      --cpp_out=${PROTO_BINARY_DIR} --grpc_out=${PROTO_BINARY_DIR}
      --plugin=protoc-gen-grpc=/home/yu/aarch64/bin/grpc_cpp_plugin
      ${PROTO_SOURCE_DIR}/${proto_file}
    MAIN_DEPENDENCY ${PROTO_SOURCE_DIR}/${proto_file}
    COMMENT "Generating Protobuf and gRPC files for ${proto_file}"
    VERBATIM)

  # 将生成的文件添加到目标
  set(GENERATED_FILES
      ${GENERATED_FILES} ${PROTO_BINARY_DIR}/${proto_file_name}.pb.cc
      ${PROTO_BINARY_DIR}/${proto_file_name}.grpc.pb.cc)
endforeach()

# 添加库 target 并链接生成的文件
add_library(face_proto ${GENERATED_FILES})
target_link_libraries(
  face_proto PUBLIC gRPC::grpc gRPC::grpc++ protobuf::libprotobuf absl::log
                    absl::strings)

# 包含目录设置
target_include_directories(face_proto PUBLIC ${PROTOBUF_INCLUDE_DIRS}
                                             ${PROTO_BINARY_DIR})
