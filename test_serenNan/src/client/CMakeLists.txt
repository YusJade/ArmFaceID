# 添加客户端源文件
set(CLIENT_SOURCES
    greeter_client.cc
    ../../generated/helloworld.pb.cc
    ../../generated/helloworld.grpc.pb.cc
)

# 添加可执行文件
add_executable(greeter_client ${CLIENT_SOURCES})

# 链接库
target_link_libraries(greeter_client
    gRPC::grpc++
    gRPC::grpc
    protobuf::libprotobuf
)
