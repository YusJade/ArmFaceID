# ArmFaceID-server

> **提醒**🍏：`gRPC`协作开发请见： [开发Tips：如何实现一个rpc接口](note/开发Tips：如何实现一个rpc接口.md)~

本 `C++` 项目旨在使用`gRPC`框架搭建一个提供人脸身份识别微服务的后端应用，提供人脸身份注册和人脸身份识别的接口。

## 构建本项目：`CMake`

> **注意**⚠️：在 `Linux` 下构建本项目。

运行以下命令进行构建：

```bash
cmake -B ./build
cmake --build ./build/
```

# 协同开发约定

- 使用 `GitHub` + `git` 进行版本管理。
  - [👑Git 关卡式学习](https://learngitbranching.js.org/?locale=zh_CN) 
- 使用 `Clion` 作为集成开发环境 `IDE`。
- 采用 `Google Cpp` 作为编码规范（会采用 `clang-format` 作为代码自动格式化工具）。
  - [📖Google 开源项目风格指南 - Cpp](https://zh-google-styleguide.readthedocs.io/en/latest/google-cpp-styleguide/contents.html)
- 使用 `CMake` 构建工程。
  - [📖CMake 保姆级教程（上）](https://subingwen.cn/cmake/CMake-primer/)
  - [📖CMake 保姆级教程（下）](https://subingwen.cn/cmake/CMake-advanced/?highlight=cmake)

## 任务划分

本分支已经在在前主分支的基础上，对`gRPC`的异步服务端的编写做了略微封装，目标是实现人脸身份注册与人脸身份识别两个接口。

### 开发并维护人脸身份识别接口

**负责开发：**
<a href="https://github.com/YusJade" ><img src="https://github.com/yusjade.png" width="50" height="50" style="border-radius: 50%; border: 2px solid #333;"/></a>

- 请求中的参数：`face_img: bytes // 要进行识别的图片`
- 响应中的参数：`res: UserInfo // 身份识别的结果`
- 响应中的消息：`数据无效：图片格式错误`，`识别不到身份`，`识别成功`，


### 并维护人脸身份注册接口

**负责开发：**
<a href="https://github.com/Serendipity-hjn" ><img src="https://github.com/Serendipity-hjn.png" width="50" height="50" style="border-radius: 50%; border: 2px solid #333;"/></a>

- 请求中的参数：
 - `face_img: bytes // 要进行注册的人脸图片`
 - `info: UserInfo // 身份信息`
- 响应中的参数：`res: User // 注册的结果`
- 响应中的消息：`数据无效：图片、身份格式错误`，`人脸图片质量过低`，`注册成功`，

## 继续开发计划

### 编写数据库模块，实现人脸特征与个人信息的绑定

已通过 `SQLITE` 数据库 与 `QT` 的 `QSQL` 模块实现基本功能，可能的待优化项：

- [ ] `Qt` 的数据库连接无法跨线程使用，解决办法为维护一个`std::thread:id`到的`map`。

### 使用性能更佳的人脸识别库

 ~~未来会尝试在开发板上部署模型，可能涉及模型的转换（`pytorch`?`onnx`?`ncnn` ?），以及运行他们所需的依赖库。~~

 本项目使用的人脸识别库 `SeetaFace2` 的人脸特征提取是基于 `ResNet50` 神经网络实现的，但并没有提供 GPU 加速的功能（源码中预留了 GPU 的代码及接口，并未实现 :<），未来计划使用 `SeetaFace6` 的 `GPU` 版本。

 <a href="https://github.com/seetafaceengine/SeetaFace6/?tab=readme-ov-file">
  <img src="https://github-readme-stats.vercel.app/api/pin/?username=seetafaceengine&repo=SeetaFace6&theme=default" alt="Liniyous/ElaWidgetTools" />
</a>

### 待办清单🧐:
- [x] 编译准备: `WSL` 下安装 `CUDA Toolkit`。
  -  [CUDA on Windows Subsystem for Linux (WSL)](https://developer.nvidia.com/cuda/wsl/)
 - [x] 编译 `X86_84 Linux GPU` 版本的 `SeetaFace6`。 
 - [x] 测试对比 `SeetaFace2` 与 `SeetaFace6` 的性能表现。
 - [x] 交叉编译 `aarch64 Linux CPU` 版本的 `SeetaFace6`。
