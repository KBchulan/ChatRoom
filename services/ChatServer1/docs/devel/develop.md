### [2025-12-12] 初始化

- 初始化项目结构

### [2025-12-15] 基本骨架搭建

#### 整体架构

```
ChatServer1/
├── include/global/          # 全局配置
│   ├── Global.hpp           # 常量定义
│   └── SuperQueue.hpp       # 无锁队列
├── src/
│   ├── core/                # 核心模块
│   │   ├── server/          # TCP 服务器入口
│   │   ├── session/         # 会话管理
│   │   ├── io/              # io_context 连接池
│   │   ├── logic/           # 逻辑处理层
│   │   └── msg-node/        # 消息节点封装
│   ├── utils/               # 工具模块
│   │   ├── common/          # 错误码定义
│   │   ├── grpc/            # gRPC 客户端
│   │   └── pool/            # 连接池
│   └── main.cc              # 入口
└── docs/devel/develop.md    # 开发文档
```

#### 核心设计

##### 1. Server (服务器入口)

- 使用 Boost.Asio 协程 (`co_spawn`) 异步接受连接
- 通过 `IO` 池轮询分配 `io_context` 给新连接
- 使用 `std::unordered_map` 管理所有 Session（带互斥锁保护）
- Pimpl 惯用法隐藏实现细节

##### 2. Session (会话层)

- 工厂方法创建，继承 `enable_shared_from_this`
- **读写分离协程**：`read_loop()` 和 `write_loop()` 使用 `||` 操作符并行运行
- 接收：固定长度头部（4字节 = 2字节消息ID + 2字节消息长度）+ 变长消息体
- 发送：使用 `SuperQueue` 无锁队列 + `steady_timer` 实现异步非阻塞发送
- 生命周期管理：
  - 客户端断开 → 协程异常 → `weak_ptr::lock()` 成功 → `RemoveSession`
  - Server 析构 → `weak_ptr::lock()` 失败 → `~_impl()` 清理

##### 3. IO (io_context 连接池)

- 单例模式，启动时创建固定数量的 `io_context`（默认 8 个）
- 每个 `io_context` 运行在独立的 `jthread` 中
- 使用 `work_guard` 保持 `io_context` 持续运行
- 轮询（Round-Robin）方式分配 `io_context`

##### 4. Logic (逻辑处理层)

- 单例模式，独立线程处理业务逻辑
- 使用 `SuperQueue` 接收来自 Session 的消息
- 使用 `atomic::wait/notify` 实现线程等待/唤醒
- 消息分发：根据 `msg_id` 查找注册的回调函数

##### 5. MsgNode (消息节点)

- `RecvNode`：接收节点，存储消息 ID 和原始数据
- `SendNode`：发送节点，自动添加网络字节序的头部

#### 消息协议

```
+------------+------------+------------------+
|  msg_id    |  msg_len   |      body        |
|  (2 bytes) |  (2 bytes) |  (msg_len bytes) |
+------------+------------+------------------+
     ^            ^
     |            |
   网络字节序   网络字节序
```

#### 当前支持的消息

| msg_id | 名称 | 说明 |
|--------|------|------|
| 1005 | ID_LOGIN_CHAT | 客户端登录请求（携带 uuid + token） |
| 1006 | ID_LOGIN_CHAT_RESPONSE | 服务端登录响应 |

#### gRPC 交互

- `StatusServerClient`：与 StatusServer 通信
  - `VerifyLoginInfo(uuid, token)`：验证客户端登录 token
- 使用 `ChannelPool` 管理 gRPC 连接（默认 8 个 channel）

#### 依赖

- Boost.Asio（协程、网络）
- gRPC + Protobuf（RPC 通信）
- jsoncpp（JSON 解析）
- spdlog/fmt（日志）

### [2025-12-16] 完善登录请求

- 引入 mariadb + redis，包括连接池以及调用，复用网关层的东西
- 逻辑层修改，查询用户信息并存入 redis 和返回给客户端

### [2025-12-18]

- 对 session.cc 修改: 选择把 self 进行值传递，同时放弃 协程 || 操作，选择单独挂一个读协程，并阻塞当前 session 进入读协程，此时才可以监听到对端关闭事件
