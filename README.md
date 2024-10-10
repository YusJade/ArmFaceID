# ArmFaceID

> **提醒**🍏：该项目为个人学习项目，仅作参考~

本 `C++` 项目旨在借助 `OpenCV` 和 `SeetaFace2` 实现**人脸检测**和**人脸识别**系统，本系统分为服务端与客户端，使用 `Protobuf` + `gRPC` 进行通信，使用 `Qt5/Qt6` 作为 `GUI`。

## 如何构建本项目：使用 `CMake`

> **注意**⚠️：仅推荐在 `Linux` 下构建本项目（只需要少量修改部分内容即可 :>），本人编写的 `CMake` 对 `Win` 平台的构建基本不存在普适性。

该项目包含以下部分：

- **proto** : 定义 `Protobuf` 的消息和 `gRPC` 的服务，该文件将由特定的生成器编译为`.cc`和`.h`文件并链接为库，供其他部分调用。
- **server** : 提供 `RPC` 服务，实现如**人脸注册**、**人脸识别**和**数据持久化**等功能。
- **client** : 通过 `OpenCV` 级联检测器实现**人脸检测**功能，使用服务器提供的`RPC`服务实现人脸的**身份识别**。
- **model** : 存放 `OpenCV`级联检测器和 `SeetaFace` 引擎所需的模型文件。
- **utils** : 一些通用方法和接口。

运行以下命令进行构建：

```bash
cmake -B ./build
cmake --build ./build/
```

## 交叉编译待办

- [x] `Qt6.5.2`: `qgrpc` 模块使用了 `v26` 版本以前的 `Protobuf` `API` ，注意本机的 `Protobuf` 版本。
- [x] `Qt5.12`
- [x] `Opencv4.10`
- [ ] `SeetaFace2` （暂不考虑）
- [x] `spdlog`: 好用的日志库🍎。
- [x] `gRPC`, `Protobuf`: 注意 `Protobuf` 的 `codegen` 和 `runtime` 的版本间隔导致的编译问题。
- [x] `ffmpeg`: 网络摄像头视频推流，用于支持 `OpenCV` 调用网络摄像头。
- [x] `x264`: 用于支持 `ffmpeg` `h264` 解码

## 继续开发计划

### 编写数据库模块，实现人脸特征与个人信息的绑定

已通过 `SQLITE` 数据库 与 `QT` 的 `QSQL` 模块实现基本功能，可能的待优化项：

- [ ] `Qt` 的数据库连接无法跨线程使用，必须在该线程下创建连接，目前采用需要时创建临时连接的方式。
- [ ] 将来在 `GUI` 中提供数据库的视图，以及一些基本的 `CRUD` 操作。

### 使用性能更佳的人脸识别库

 ~~未来会尝试在开发板上部署模型，可能涉及模型的转换（`pytorch`?`onnx`?`ncnn` ?），以及运行他们所需的依赖库。~~

 本项目使用的人脸识别库 `SeetaFace2` 的人脸特征提取是基于 `ResNet50` 神经网络实现的，但并没有提供 GPU 加速的功能（源码中预留了 GPU 的代码及接口，并未实现 :<），未来计划使用 `SeetaFace6` 的 `GPU` 版本。

 <a href="https://github.com/seetafaceengine/SeetaFace6/?tab=readme-ov-file">
  <img src="https://github-readme-stats.vercel.app/api/pin/?username=seetafaceengine&repo=SeetaFace6&theme=default" alt="Liniyous/ElaWidgetTools" />
</a>

#### 待办清单🧐:
- [x] 编译准备: `WSL` 下安装 `CUDA Toolkit`。
  -  [CUDA on Windows Subsystem for Linux (WSL)](https://developer.nvidia.com/cuda/wsl/)
 - [x] 编译 `X86_84 Linux GPU` 版本的 `SeetaFace6`。 
 - [ ] 测试对比 `SeetaFace2` 与 `SeetaFace6` 的性能表现。
 - [ ] 交叉编译 `aarch64 Linux CPU` 版本的 `SeetaFace6`。

### 集成更美观的 Qt-GUI：ElaWidgetTools

<a href="https://github.com/Liniyous/ElaWidgetTools?tab=readme-ov-file">
  <img src="https://github-readme-stats.vercel.app/api/pin/?username=Liniyous&repo=ElaWidgetTools&theme=default" alt="Liniyous/ElaWidgetTools" />
</a>

服务端已使用该 UI 库。