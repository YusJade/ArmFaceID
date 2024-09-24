# ArmFaceID

> 提醒🍏：该项目为个人学习项目，仅作参考~

本 `C++` 项目旨在借助 `OpenCV` 和 `SeetaFace2` 实现**人脸检测**和**人脸识别**系统，本系统分为服务端与客户端，使用 `Protobuf` + `gRPC` 进行通信，使用 `Qt5/Qt6` 作为 `GUI`。

## 如何构建本项目：使用 `CMake`

> 注意⚠️：仅推荐在 `Linux` 下构建本项目（只需要少量修改部分内容即可 :>），本人编写的 `CMake` 对 `Win` 平台的构建基本不存在普适性。

该项目包含以下部分：

- **proto** : 定义 `Protobuf` 的消息和 `gRPC` 的服务，该文件将由特定的生成器编译为`.cc`和`.h`文件并链接为库，供其他部分调用。
- **server** : 提供 `RPC` 服务，实现如**人脸注册**、**人脸识别**和**数据持久化**等功能。
- **client** : 通过 `OpenCV` 级联检测器实现**人脸检测**功能，使用服务器提供的`RPC`服务实现人脸的**身份识别**。
- **model** : 存放 `OpenCV`级联检测器和 `SeetaFace` 引擎所需的模型文件。

运行以下命令进行构建：

```bash
cmake -B ./build
cmake --build ./build/
```

## 交叉编译待办

  - [ ] `Qt6.5.2`: `qgrpc` 模块使用了 `v26` 版本以前的 `Protobuf` `API` ，注意本机的 `Protobuf` 版本。
  - [x] `Qt5.12`
  - [x] `Opencv4.10`
  - [ ] `SeetaFace2` （暂不考虑）
  - [x] `spdlog`: 好用的日志库🍎。
  - [x] `gRPC`, `Protobuf`: 注意 `Protobuf` 的 `codegen` 和 `runtime` 的版本间隔导致的编译问题。
  - [x] `ffmpeg`: 网络摄像头视频推流，用于支持 `OpenCV` 调用网络摄像头。
  - [x] `x264`: 用于支持 `ffmpeg` `h264` 解码 
  
## 继续开发计划

### 在开发板上部署深度学习模型

未来会尝试在开发板上部署模型，可能涉及模型的转换（`pytorch`?`onnx`?`ncnn` ?），以及运行他们所需的依赖库。

### 集成更美观的 Qt-GUI：ElaWidgetTools


<a href="https://github.com/Liniyous/ElaWidgetTools?tab=readme-ov-file">
  <img src="https://github-readme-stats.vercel.app/api/pin/?username=Liniyous&repo=ElaWidgetTools&theme=default" alt="Repo A" />
</a>

根据 `repo` 介绍，该 `UI` 库对 `Qt6` 支持良好，交叉编译 `Qt6` 成功后考虑纳入计划~
