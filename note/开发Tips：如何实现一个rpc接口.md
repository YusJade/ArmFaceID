## 开发Tips：如何实现一个`rpc`接口

[🔗gRPC | Quick start 快速上手](https://grpc.io/docs/languages/cpp/quickstart/)
[🔗gRPC | Asynchronous-API tutorial 异步-API 向导](https://grpc.io/docs/languages/cpp/async/)

本工程对 `gRPC` 做了一些封装......
假设需要实现一个 `rpc` 接口 ：

``` cpp
rpc RpcRecognizeFace(RecognitionRequest) returns (RecognitionResponse) {}
```

该接口接受一个类型为`RecognitionRequest`请求消息，返回一个类型为 `RecognitionResponse` 响应消息。

那你需要：
 
- 注册异步响应 `RpcHandler`。
- 向`RpcHandler`注册一个处理请求的回调方法。

### 注册异步响应 `RpcHandler`

在 `third_party/grpc/src/rpc_handler.cpp` 中添加如下的*函数模板*的*特化*版本：

```cpp
void RequestRpc(FaceRpc::AsyncService* service, grpc::ServerContext* ctx,
                RecognitionRequest& req,
                grpc::ServerAsyncResponseWriter<RecognitionResponse>& resp,
                ::grpc::CompletionQueue* new_call_cq,
                ::grpc::ServerCompletionQueue* notification_cq, void* tag) {
  // service->RequestXXX 中的 XXX 修改为 .proto 中对应的接口名
  service->RequestRpcRecognizeFace(ctx, &req, &resp, new_call_cq,
                                   notification_cq, tag);
}
```

这会使得 `gRPC` 能够将异步请求交付给 `RpcHandler` 处理......

### 向`RpcHandler`注册一个处理请求的回调方法

注册回调的方式是调用`RpcServer`实例的`RegisterRPCHandler`方法，这是一个类成员函数模板：

```cpp
// 注册请求处理的回调方法。
  server.RegisterRPCHandler<RecognitionRequest, RecognitionResponse>(
      [=](RecognitionRequest& req, /* 从客户端接受到的请求消息 */
          RecognitionResponse& resp /* 将要返回的响应消息 */) {
        auto resp_begin_time_point = std::chrono::high_resolution_clock::now();

        /**
           在这里实现对应的业务逻辑 :>
        */
        auto resp_finish_time_point = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> took_time =
            resp_finish_time_point - resp_begin_time_point;
        SPDLOG_INFO("处理一条xxx请求，耗时 {} s", took_time.count());

        return grpc::Status(grpc::StatusCode::OK, "成功");
      });
```