# ChatRoom

## 快速开始

可以选择直接用 Qt Creator 打开 `CMakeLists.txt` 构建，或者使用命令行：

```bash
cd chatroom
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
./build/chatroom
```

## 开发文档

详细开发流程请参考 [docs/develop.md](docs/develop.md)。
