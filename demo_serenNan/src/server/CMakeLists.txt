# 添加服务端源文件
set(SERVER_SOURCES
    greeter_server.cc
    ../../proto/build/helloworld.pb.cc
    ../../proto/build/helloworld.grpc.pb.cc
)

# 添加可执行文件
add_executable(greeter_server ${SERVER_SOURCES})

# 链接库
target_link_libraries(greeter_server
    gRPC::grpc++
    gRPC::grpc
    protobuf::libprotobuf
)
