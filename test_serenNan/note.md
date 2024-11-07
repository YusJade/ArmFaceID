# grpc示例




## 错误处理

### cmake时找不到对应的包
```text
CMake Error at CMakeLists.txt:8 (find_package):
  Found package configuration file:

    /usr/local/lib/cmake/grpc/gRPCConfig.cmake

  but it set gRPC_FOUND to FALSE so package "gRPC" is considered to be NOT
  FOUND.  Reason given by package:

  The following imported targets are referenced, but are missing:
  protobuf::libprotobuf protobuf::libprotoc
```
protobuf和grpc的find_package顺序需要调整，先找protobuf再找grpc
```CMakeLists.txt
find_package(protobuf REQUIRED)
find_package(gRPC REQUIRED)
```

### make时找不到对应的库(absl)
