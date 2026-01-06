# ChatRoom ChatServer

这是 ChatRoom 分布式聊天系统的聊天服务器，基于 Boost.Asio 协程构建的高性能 TCP 长连接服务器，负责用户登录校验、会话管理和实时消息处理。

## 快速开始

### 环境要求

我是基于原来做的 [模板项目](https://github.com/KBchulan/CMakeTemplate) 进行开发的，除了该模板项目的依赖外，还需要以下环境：

| 依赖         | 版本    | 用途                |
| ------------ | ------- | ------------------- |
| CMake        | 3.25+   | 构建系统            |
| GCC/Clang    | 13+/16+ | 支持 C++23 的编译器 |
| Boost        | 1.81+   | Asio 协程           |
| gRPC         | 1.76+   | 微服务通信          |
| Protobuf     | 33.1+   | 序列化              |
| MariaDB      | 12.1+   | 持久化存储          |
| mariadb-libs | 12.1+   | MariaDB 客户端库    |
| Redis        | 8.1+    | 缓存                |
| hiredis      | 1.3+    | Redis 客户端        |
| fmt          | 12.1+   | 格式化输出          |
| JsonCpp      | 1.9+    | JSON 解析           |

可以使用自带的包管理器安装，按照标准可以参考 [ci流程](../../.github/workflows/chat-server.yml) 中的安装步骤，该流程在 github actions 中已经通过 ubuntu:latest 的校验。

### 修改配置

可以查看一下 [服务器配置文件](./include/global/Global.hpp)，根据需要修改相关配置项，比如端口号、线程池大小、数据库相关配置等。

### 本地开发

本地快速启动聊天服务器，可以参考下面的步骤，这个默认启动了 ASan 和 UBSan，会进行内存泄露和未定义行为的检测：

```bash
cmake -S . -B build
cmake --build build

# 确保 StatusServer、MariaDB 和 Redis 已启动
./build/bin/ChatServer
```

### 命令行参数

ChatServer 支持以下命令行参数：

```bash
Usage: ChatServer [-h] [-p <port>]
Options:
  -h, --help         显示帮助信息
  -p, --port <port>  服务器端口 (默认: 10004)
```

**示例**：

```bash
# 使用默认端口 10004 启动
./build/bin/ChatServer

# 指定端口 10006 启动（用于集群部署）
./build/bin/ChatServer -p 10006
```

更多构建配置可以参考 [指引指南](./docs/guide/README.md)。

## 项目介绍

### 目录说明

下面是本项目的目录结构说明，整体采用分层架构风格：

```
ChatServer/
├── src/
│   ├── main.cc                     # 程序入口，初始化各组件
│   ├── core/
│   │   ├── server/                 # 多 Acceptor 协程架构服务器
│   │   ├── session/                # TCP 会话管理
│   │   ├── msg-node/               # 消息节点 (发送/接收)
│   │   ├── logic/                  # 业务逻辑系统
│   │   ├── io/                     # io_context 池
│   │   ├── repository/             # 数据访问层
│   │   └── model/                  # 领域模型
│   └── utils/
│       ├── common/                 # 通用工具 (错误码/消息ID)
│       ├── db_params/              # MySQL 参数绑定辅助
│       ├── pool/                   # 连接池 (MariaDB/Redis/gRPC)
│       └── grpc/                   # gRPC 客户端和 IDL 生成代码
├── include/
│   ├── global/                     # 全局配置与无锁队列
│   ├── tools/                      # 工具组件 (日志/ID生成/Defer)
│   └── config/                     # 读取 cmake 全局变量
└── docs/                           # 文档和指引文档
```

### 核心架构

下面展示了聊天服务器的核心架构设计图：

```
                              ┌─────────────────────────────────────────┐
                              │            ChatServer                   │
                              ├─────────────────────────────────────────┤
    ┌──────────┐              │  ┌─────────────────────────────────┐    │
    │  Client  │────TCP─────▶│  │   Multi-Acceptor (SO_REUSEPORT) │    │
    └──────────┘              │  │   协程风格，内核级负载均衡          │    │
                              │  └──────────────┬──────────────────┘    │
                              │                 │                       │
                              │  ┌──────────────▼──────────────────┐    │
                              │  │        IO Context Pool          │    │
                              │  │     (8 threads, 网络 I/O 池子)   │    │
                              │  └──────────────┬──────────────────┘    │
                              │                 │                       │
                              │  ┌──────────────▼──────────────────┐    │
                              │  │           Session               │    │
                              │  │  协程读写循环 + 无锁发送队列        │    │
                              │  └──────────────┬──────────────────┘    │
                              │                 │                       │
                              │  ┌──────────────▼──────────────────┐    │
                              │  │       Logic (业务线程)           │    │
                              │  │  SuperQueue → Handler 分发       │    │
                              │  └──────────────┬──────────────────┘    │
                              │                 │                       │
                              │  ┌──────────────▼──────────────────┐    │
                              │  │         Repository              │    │
                              │  └──────────────┬──────────────────┘    │
                              └───────────────────┬─────────────────────┘
                                                  │
                ┌─────────────────────────────────┼─────────────────────────────┐
                │                                 │                             │
                ▼                                 ▼                             ▼
        ┌───────────────┐                 ┌───────────────┐             ┌───────────────┐
        │   MariaDB     │                 │    Redis      │             │  gRPC Client  │
        │  (连接池 16)   │                 │  (连接池 16)   │             │ (Channel 池)  │
        └───────────────┘                 └───────────────┘             └───────┬───────┘
                                                                                │
                                                                                ▼
                                                                        ┌───────────────┐
                                                                        │ StatusServer  │
                                                                        │  状态管理服务   │
                                                                        │   (登录校验)   │
                                                                        └───────────────┘
```

### 通信协议

ChatServer 使用经典 TLV 协议进行通信：

```
┌──────────────────────────────────────────────────────────┐
│                      消息格式                             │
├──────────────┬──────────────┬───────────────────────────-┤
│  msg_id (2B) │  msg_len (2B)│       body (变长请求体)     │
│   网络字节序   │   网络字节序  │        由数据序列化即可       │
└──────────────┴──────────────┴────────────────────────────┘
```

| 字段    | 长度 | 说明                                |
| ------- | ---- | ----------------------------------- |
| msg_id  | 2B   | 消息类型 ID，网络字节序             |
| msg_len | 2B   | 消息体长度，网络字节序              |
| body    | 变长 | 请求体的实际内容，支持 json、xml 等 |

### 设计亮点

#### 1. 高性能自研组件

| 组件                   | 描述          | 吞吐量     |
| ---------------------- | ------------- | ---------- |
| **SuperQueue**   | MPMC 无锁队列 | 100M+/s    |
| **Logger**       | 异步日志系统  | 260M+/s    |
| **ID Generator** | 雪花算法/UUID | 40M/23M+/s |

#### 2. 多 Acceptor + 协程架构

不同于网关的回调风格，ChatServer 保留了协程风格，同时实现多 Acceptor：

- 每个 `io_context` 线程拥有独立的 Acceptor
- 使用 `SO_REUSEPORT` 允许多个 socket 绑定同一端口
- 每个 AcceptorWorker 使用 `co_spawn` 启动协程 accept 循环

```cpp
// server.cc:43 - 协程风格的 accept
boost::asio::co_spawn(_io_context, accept_loop(), boost::asio::detached);
```

#### 3. 协程读写分离

Session 使用两个独立协程分别处理读写，通过无锁队列解耦：

```cpp
// session.cc:112-114
boost::asio::co_spawn(_socket.get_executor(), self->_pimpl->write_loop(self), boost::asio::detached);
co_await self->_pimpl->read_loop(self);
```

- **read_loop**: 持续读取消息头和消息体，投递到 Logic 队列
- **write_loop**: 从 SuperQueue 取消息发送，无消息时定时器挂起

#### 4. 无锁业务队列

Logic 系统使用 SuperQueue 实现零拷贝消息传递，从而实现消息的高效处理，目前 logic 层选择单线程处理消息，因为作为聊天服务器处理请求是很轻量的，加锁不值得：

```cpp
// logic.cc:63-66 - 无锁消费
if (_queue.pop(task))
{
  handle_message(task.session, task.msg);
  continue;
}
_has_task.wait(false, std::memory_order_acquire);
```

#### 5. 连接池设计

MariaDB 和 Redis 连接池采用统一的 RAII 设计：

- **固定大小数组 + 原子位图**：预分配连接，避免动态内存分配
- **`atomic::wait/notify`**：C++20 原子等待，比条件变量更轻量
- **连接失效自动重连**：通过 `mysql_ping` / `PING` 检测并恢复

```cpp
// db_pool.cc:226 - 无锁获取连接
if (mysql_ping(_slots[i]._conn) != 0)
{
  mysql_close(_slots[i]._conn);
  _slots[i]._conn = create_connection();
}
```

#### 6. Pimpl 惯用法

所有核心类都采用 Pimpl（编译防火墙）模式：

- **ABI 稳定**：私有成员变更不影响二进制兼容性
- **编译加速**：减少头文件依赖，降低编译时间
- **实现隐藏**：对外只暴露 `std::unique_ptr<_impl>`

```cpp
// session.hpp
class Session : public std::enable_shared_from_this<Session> {
public:
  void Start();
private:
  struct _impl;
  std::unique_ptr<_impl> _pimpl;
};
```

#### 7. 优雅的数据库参数绑定

使用变参模板简化 prepared statement 参数绑定：

```cpp
// user_repository.cc:18-19
auto params = utils::MakeParams(userId);
auto results = utils::MakeResults(nickname, avatar, email);
conn.QueryOne(sql, params, results);
```

#### 8. 安全错误处理

基于 `std::expected` 替代异常，零开销错误传递：

```cpp
// status_server_client.hpp:30
using LoginVerifyResult = std::expected<LoginVerifyResponse, GrpcError>;

// 调用示例
auto res = _status_server_client.VerifyLoginInfo(uuid, token);
if (!res) {
    // 处理错误
    response["code"] = res.error().code;
}
```

### 模块说明

首先对于 `core` 目录下的模块进行说明：

| 模块                 | 说明                                     |
| -------------------- | ---------------------------------------- |
| **Server**     | 多 Acceptor 协程架构入口，监听端口 10004 |
| **IO**         | io_context 池，Round-Robin 分配连接      |
| **Session**    | TCP 会话对象，协程读写 + 无锁发送队列    |
| **MsgNode**    | 消息节点，RecvNode 和 SendNode           |
| **Logic**      | 业务逻辑系统，无锁队列 + Handler 分发    |
| **Repository** | 数据访问层，封装数据库操作               |
| **Model**      | 领域模型，一些数据结构的定义             |

其次是 `utils` 目录下的工具模块：

| 模块                  | 说明                                   |
| --------------------- | -------------------------------------- |
| **DBPool**      | MariaDB 连接池，预分配 + 原子操作      |
| **RedisPool**   | Redis 连接池，支持全数据结构操作       |
| **ChannelPool** | gRPC Channel 复用池，给 rpc 客户端使用 |
| **gRPC**        | protobuf 代码生成与 rpc 客户端封装     |
| **db_params**   | MySQL 参数绑定辅助，支持类型安全绑定   |
| **common**      | 错误码和消息 ID 定义                   |

最后是 `include` 目录下的全局配置与工具组件：

| 模块             | 说明                              |
| ---------------- | --------------------------------- |
| **config** | 读取 CMake 全局变量配置           |
| **global** | 全局配置与无锁队列 SuperQueue     |
| **tools**  | 工具组件，日志、ID 生成、Defer 等 |

### 消息 ID 定义

当前已实现的消息类型：

| 消息 ID | 名称                   | 说明         |
| ------- | ---------------------- | ------------ |
| 1005    | ID_LOGIN_CHAT          | 逻辑登录请求 |
| 1006    | ID_LOGIN_CHAT_RESPONSE | 逻辑登录响应 |

## 开发文档

详细的开发进度和变更记录请参考 [develop.md](./docs/devel/develop.md)。

## TODO

- [ ] 消息路由与转发
- [ ] 群聊功能
- [ ] 消息持久化
- [ ] 心跳检测
