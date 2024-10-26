# 配置并构建 face_proto 生成 cpp 代码
# cmake -DPREWORK_CROSS_COMPILE=ON -B ./aarch64-build .
# cmake --build ./aarch64-build --target face_proto
# 执行交叉编译
cmake -DCMAKE_TOOLCHAIN_FILE=./aarch64.cmake -B ./aarch64-build .
cmake --build ./aarch64-build --target client