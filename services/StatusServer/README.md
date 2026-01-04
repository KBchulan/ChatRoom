# ChatRoom StatusServer

这是 ChatRoom 分布式聊天系统的状态服务器，基于 gRPC 构建的轻量级服务发现与负载均衡组件，负责 ChatServer 集群的服务发现、连接分配和登录校验。

## 快速开始

### 环境要求

我是基于原来做的 [模板项目](https://github.com/KBchulan/CMakeTemplate) 进行开发的，除了该模板项目的依赖外，还需要以下环境：

| 依赖      | 版本    | 用途                |
| --------- | ------- | ------------------- |
| CMake     | 3.25+   | 构建系统            |
| GCC/Clang | 13+/16+ | 支持 C++23 的编译器 |
| gRPC      | 1.76+   | RPC 框架            |
| Protobuf  | 33.1+   | 序列化              |
| fmt       | 12.1+   | 格式化输出          |

可以使用自带的包管理器安装，按照标准可以参考 [ci流程](../../.github/workflows/status-server.yml) 中的安装步骤，该流程在 github actions 中已经通过 ubuntu:latest 的校验。

### 修改配置

可以查看一下 [服务器配置文件](./include/global/Global.hpp)，根据需要修改相关配置项，如果需要横向扩展 ChatServer，可以修改 `_tcp_servers` 列表，添加更多服务器地址和端口。

### 本地开发

本地快速启动状态服务器，可以参考下面的步骤，这个默认启动了 ASan 和 UBSan，会进行内存泄露和未定义行为的检测：

```bash
cmake -S . -B build
cmake --build build

./build/bin/StatusServer
```

更多构建配置可以参考 [指引指南](./docs/guide/README.md)。

## 项目介绍

### 目录说明

下面是本项目的目录结构说明：

```
StatusServer/
├── src/
│   ├── main.cc                     # 程序入口
│   ├── core/
│   │   └── server/                 # gRPC 服务实现
│   └── utils/
│       └── gen/                    # protobuf 生成代码
├── include/
│   ├── global/                     # 全局配置与无锁队列
│   ├── tools/                      # 工具组件 (日志/ID生成/Defer)
│   └── config/                     # 读取 cmake 全局变量
├── tests/                          # 单元测试
├── benchmark/                      # 基准测试
└── docs/                           # 文档
```

### 核心架构

下面展示了状态服务器在整个系统中的位置：

```
                              ┌─────────────────────────────────────────┐
                              │            StatusServer                 │
                              │            (gRPC :10003)                │
                              ├─────────────────────────────────────────┤
    ┌──────────┐              │                                         │
    │ GateWay  │───gRPC─────▶ │  ┌─────────────────────────────────┐    │
    └──────────┘              │  │      StatusServiceImpl          │    │
         │                    │  │                                 │    │
         │                    │  │  ┌───────────────────────────┐  │    │
         │                    │  │  │  GetTcpServer()           │  │    │
         │                    │  │  │  - 选择最小负载 ChatServer  │  │    │
         │                    │  │  │  - 生成一次性 Token        │  │    │
         │                    │  │  └───────────────────────────┘  │    │
         │                    │  │                                 │    │
         │                    │  │  ┌───────────────────────────┐  │    │
    ┌────────────┐            │  │  │  LoginVerify()            │  │    │
    │ ChatServer │──gRPC────▶ │  │  │  - 校验 Token 有效性       │  │    │
    └────────────┘            │  │  │  - 更新连接计数            │  │    │
                              │  │  └───────────────────────────┘  │    │
                              │  │                                 │    │
                              │  └─────────────────────────────────┘    │
                              │                                         │
                              │  ┌─────────────────────────────────┐    │
                              │  │     TcpServerInfo[]             │    │
                              │  │  ┌─────────┐  ┌─────────┐       │    │
                              │  │  │ :10004  │  │ :10005  │  ...  │    │
                              │  │  │ count=5 │  │ count=3 │       │    │
                              │  │  └─────────┘  └─────────┘       │    │
                              │  └─────────────────────────────────┘    │
                              └─────────────────────────────────────────┘
```

### RPC 接口定义

StatusServer 提供两个 gRPC 接口：

```proto
service StatusService {
  // 获取负载最小的 ChatServer 地址
  rpc GetTcpServer(GetTcpServerRequest) returns (GetTcpServerResponse);

  // 校验登录 Token
  rpc LoginVerify(LoginVerifyRequest) returns (LoginVerifyResponse);
}
```

#### GetTcpServer

获取当前负载最小的 ChatServer 地址，同时生成一次性 Token。

| 字段           | 类型   | 说明                |
| -------------- | ------ | ------------------- |
| **请求** |        |                     |
| uuid           | string | 用户唯一标识        |
| **响应** |        |                     |
| code           | int32  | 状态码，0 表示成功  |
| message        | string | 状态消息            |
| data.host      | string | ChatServer 主机地址 |
| data.port      | string | ChatServer 端口     |
| data.token     | string | 一次性登录 Token    |

#### LoginVerify

校验用户登录 Token，验证通过后更新连接计数。

| 字段           | 类型   | 说明                      |
| -------------- | ------ | ------------------------- |
| **请求** |        |                           |
| uuid           | string | 用户唯一标识              |
| token          | string | GetTcpServer 返回的 Token |
| **响应** |        |                           |
| code           | int32  | 状态码，0 表示成功        |
| message        | string | 状态消息                  |

### 设计亮点

#### 1. 最小负载选择算法

使用 `std::ranges::min_element` 实现 O(n) 复杂度的最小负载选择：

```cpp
// server.cc:44 - 选择连接数最少的服务器
auto iter = std::ranges::min_element(_tcp_servers, {}, &TcpServerInfo::connection_count);
```

#### 2. 两阶段 Token 验证

防止重放攻击的安全机制，同时支持**请求幂等性**（相同 uuid 重复请求返回相同结果）：

```
┌────────┐         ┌─────────┐         ┌──────────────┐         ┌────────────┐
│ Client │         │ GateWay │         │ StatusServer │         │ ChatServer │
└───┬────┘         └────┬────┘         └──────┬───────┘         └─────┬──────┘
    │                   │                     │                       │
    │   1. 登录请求      │                     │                       │
    │─────────────────▶│                     │                       │
    │                   │   2. GetTcpServer   │                       │
    │                   │───────────────────▶│                       │
    │                   │                     │  检查是否已分配        │
    │                   │                     │  若已分配则直接返回    │
    │                   │                     │  否则生成新 Token      │
    │                   │                     │  记录 uuid→token      │
    │                   │◀───────────────────│                       │
    │                   │   {host,port,token} │                       │
    │◀─────────────────│                     │                       │
    │   返回服务器信息    │                     │                       │
    │                   │                     │                       │
    │              3. TCP 连接 + Token        │                       │
    │───────────────────────────────────────────────────────────────▶│
    │                   │                     │   4. LoginVerify      │
    │                   │                     │◀─────────────────────│
    │                   │                     │   校验 Token           │
    │                   │                     │   增加连接计数          │
    │                   │                     │─────────────────────▶│
    │                   │                     │   验证结果             │
    │◀───────────────────────────────────────────────────────────────│
    │                   │                     │   登录成功/失败        │
```

- **阶段一**：GateWay 调用 `GetTcpServer`，获取 Token 和服务器地址
- **阶段二**：ChatServer 调用 `LoginVerify`，验证 Token 并更新计数

Token 一次性有效，验证后立即删除，防止重复使用。

#### 3. 线程安全设计

使用 `std::mutex` 保护共享状态：

```cpp
// server.cc:41-53
std::lock_guard lock(_mutex);

// 选择服务器、生成 Token、存储映射
auto iter = std::ranges::min_element(_tcp_servers, {}, &TcpServerInfo::connection_count);
_pending_connections[uuid] = {.token = token, .server_index = server_index};
```

#### 4. 优雅的信号处理

使用条件变量实现优雅关闭：

```cpp
// main.cc:51-57
std::thread shutdown_thread([&server]() {
  std::unique_lock lock(g_mutex);
  g_cv.wait(lock, []() { return g_received_signal.load() != 0; });
  server->Shutdown();
});
```

### 模块说明

| 模块                         | 说明                                 |
| ---------------------------- | ------------------------------------ |
| **StatusServiceImpl**  | gRPC 服务实现，负载均衡与 Token 校验 |
| **TcpServerInfo**      | ChatServer 信息，包含连接计数        |
| **UserConnectionInfo** | 待验证连接信息，Token 与服务器索引   |

### 调用方说明

| 调用方               | 调用接口     | 场景                           |
| -------------------- | ------------ | ------------------------------ |
| **GateWay**    | GetTcpServer | 用户登录时获取 ChatServer 地址 |
| **ChatServer** | LoginVerify  | 用户连接时校验 Token 有效性    |

## 开发文档

详细的开发进度和变更记录请参考 [develop.md](./docs/devel/develop.md)。

## TODO

- [ ] 支持动态注册/注销 ChatServer
- [ ] 连接数定期同步（心跳机制）
- [ ] 支持权重配置的负载均衡
- [ ] 服务健康检查
