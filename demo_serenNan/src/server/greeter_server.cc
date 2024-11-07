#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "helloworld.grpc.pb.h"

// 使用命名空间 example 中的 Greeter、HelloReply 和 HelloRequest 类
using example::Greeter;
using example::HelloReply;
using example::HelloRequest;
// 使用命名空间 grpc 中的 Server、ServerBuilder、ServerContext 和 Status 类
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

// 实现 Greeter 服务
class GreeterServiceImpl final : public Greeter::Service {
  // 重写 SayHello 方法，处理客户端的请求
  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    // 定义一个字符串前缀 "Hello "
    std::string prefix("Hello ");
    // 将前缀和请求中的名字拼接，并设置到回复消息中
    reply->set_message(prefix + request->name());
    // 返回状态 OK，表示操作成功
    return Status::OK;
  }
};

// 运行服务器的函数
void RunServer() {
  // 定义服务器地址，使用 "0.0.0.0:50051"
  std::string server_address("0.0.0.0:50051");
  // 创建 GreeterServiceImpl 类的实例
  GreeterServiceImpl service;

  // 创建 ServerBuilder 类的实例
  ServerBuilder builder;
  // 添加监听端口，使用不安全的认证方式
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // 注册服务
  builder.RegisterService(&service);
  // 构建并启动服务器
  std::unique_ptr<Server> server(builder.BuildAndStart());
  // 输出服务器正在监听的地址
  std::cout << "Server listening on " << server_address << std::endl;

  // 等待服务器结束
  server->Wait();
}

// 主函数
int main(int argc, char** argv) {
  // 运行服务器
  RunServer();
  // 返回 0，表示程序正常结束
  return 0;
}