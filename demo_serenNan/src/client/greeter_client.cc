#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "helloworld.grpc.pb.h"

// 使用命名空间中的类和函数
using example::Greeter;
using example::HelloReply;
using example::HelloRequest;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

// 定义一个客户端类，用于与gRPC服务进行通信
class GreeterClient {
 public:
  // 构造函数，接受一个Channel指针，并创建一个Stub对象
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  // 定义一个方法，用于向服务端发送请求并接收响应
  std::string SayHello(const std::string& user) {
    // 创建一个HelloRequest对象，并设置其name字段
    HelloRequest request;
    request.set_name(user);

    // 创建一个HelloReply对象，用于存储服务端的响应
    HelloReply reply;
    // 创建一个ClientContext对象，用于设置请求的上下文
    ClientContext context;

    // 调用Stub对象的SayHello方法，发送请求并接收响应
    Status status = stub_->SayHello(&context, request, &reply);

    // 检查gRPC调用是否成功
    if (status.ok()) {
      // 如果成功，返回服务端返回的消息
      return reply.message();
    } else {
      // 如果失败，打印错误码和错误信息，并返回一个错误消息
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  // 定义一个Stub对象，用于与gRPC服务进行通信
  std::unique_ptr<Greeter::Stub> stub_;
};

// 主函数，程序的入口
int main(int argc, char** argv) {
  // 创建一个GreeterClient对象，连接到本地的gRPC服务
  GreeterClient greeter(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
  // 定义一个字符串变量，存储用户名
  std::string user("world");
  // 调用SayHello方法，发送请求并接收响应
  std::string reply = greeter.SayHello(user);
  // 打印服务端返回的消息
  std::cout << "Greeter received: " << reply << std::endl;

  // 程序正常结束
  return 0;
}