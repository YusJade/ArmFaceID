# 示例

在项目根目录下运行以下命令：
```bash
mkdir -p build
cd build
cmake ..
make
```

然后先运行
```bash
./src/server/greeter_server 
```
出现
```bash
Server listening on 0.0.0.0:5005
```
表示服务端启动成功，客户端连接成功。

新开一个终端再运行
```bash
./src/client/greeter_client
```
出现
```bash
Greeter received: Hello world
```

# 错误处理

## CMake Error

### cmake时找不到对应的包
```CMake
CMake Error at CMakeLists.txt:8 (find_package):
  Found package configuration file:

    /usr/local/lib/cmake/grpc/gRPCConfig.cmake

  but it set gRPC_FOUND to FALSE so package "gRPC" is considered to be NOT
  FOUND.  Reason given by package:

  The following imported targets are referenced, but are missing:
  protobuf::libprotobuf protobuf::libprotoc
```

**解决方案：**
protobuf和grpc的find_package顺序需要调整，先找protobuf再找grpc
```CMakeLists.txt
find_package(protobuf REQUIRED)
find_package(gRPC REQUIRED)
```

## make Error
### make时找不到对应的库(absl)
```make
/usr/local/include/grpc/event_engine/endpoint_config.h:36:17: error: ‘optional’ in namespace ‘absl’ does not name a template type
   36 |   virtual absl::optional<int> GetInt(absl::string_view key) const = 0;
      |                 ^~~~~~~~
```

**解决方案：**
这是标准问题，顶层`CMakeLists.txt`的`C++`改成17以上
```CMakeLists.txt
set(CMAKE_CXX_STANDARD 17)
```