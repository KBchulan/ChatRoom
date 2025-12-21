# ChatRoom GateWay

这是 ChatRoom 分布式聊天系统的网关服务，基于 Boost.Beast 构建的高性能 HTTP 网关，负责流量入口、身份认证、限流和请求路由。

## 快速开始

### 环境要求

我是基于原来做的 [模板项目](https://github.com/KBchulan/CMakeTemplate) 进行开发的，除了该模板项目的依赖外，还需要以下环境：

| 依赖         | 版本    | 用途                |
| ------------ | ------- | ------------------- |
| CMake        | 3.25+   | 构建系统            |
| GCC/Clang    | 13+/16+ | 支持 C++23 的编译器 |
| Boost        | 1.81+   | Asio/Beast/URL      |
| gRPC         | 1.76+   | 微服务通信          |
| Protobuf     | 33.1+   | 序列化              |
| MariaDB      | 12.1+   | 持久化存储          |
| mariadb-libs | 12.1+   | MariaDB 客户端库    |
| Redis        | 8.1+    | 缓存/限流           |
| hiredis      | 1.3+    | Redis 客户端        |
| jwt-cpp      | -       | JWT 认证            |
| libsodium    | 1.0+    | Argon2id 密码哈希   |
| fmt          | 12.1+   | 格式化输出          |
| JsonCpp      | 1.9+    | JSON 解析           |

可以使用自带的包管理器安装，按照标准可以参考 [ci流程](../../.github/workflows/gateway.yml) 中的安装步骤，该流程在 github actions 中已经通过 ubuntu:latest 的校验。

### 修改配置

可以查看一下 [网关的配置文件](./include/global/Global.hpp)，根据需要修改相关配置项，比如端口号、线程池大小、限流参数、数据库相关配置等。

### 本地开发

本地快速启动网关，可以参考下面的步骤，这个默认启动了 ASan 和 UBSan，会进行内存泄露和未定义行为的检测：

```bash
cmake -S . -B build
cmake --build build

# 确保 StatusServer、MariaDB 和 Redis 已启动，启动网关
./build/bin/GateWay
```

更多构建配置可以参考 [指引指南](./docs/guide/README.md)。

## 项目介绍

### 目录说明

下面是本项目的目录结构说明，方便对着目录理解，整体来说就是 socket + ddd 风格的目录：

```
GateWay/
├── src/
│   ├── main.cc                     # 程序入口，初始化各组件
│   ├── core/
│   │   ├── server/                 # 多 Acceptor 架构服务器
│   │   ├── connection/             # HTTP 连接处理
│   │   ├── logic/                  # 路由注册与分发
│   │   ├── io/                     # io_context 池
│   │   ├── business/               # 业务线程池
│   │   ├── controller/             # 控制器层
│   │   ├── service/                # 服务层
│   │   ├── repository/             # 数据访问层
│   │   └── domain/                 # 领域模型 (DTO/VO/DO)
│   └── utils/
│       ├── common/                 # 通用工具 (错误码/通用函数/JWT/限流/路由)
|       ├── db_params/              # 构造数据库参数
│       ├── pool/                   # 连接池 (MariaDB/Redis/gRPC)
│       ├── grpc/                   # gRPC 客户端和 IDL 生成代码
|       ├── url/                    # URL 解析工具
│       └── context/                # 请求上下文
├── include/
│   ├── global/                     # 全局配置与无锁队列
│   ├── tools/                      # 工具组件 (日志/ID生成/Defer)
│   └── config/                     # 读取 cmake 全局变量
├── tests/                          # 单元测试
├── benchmark/                      # 基准测试
└── docs/                           # 开发文档和指引文档
```

### 核心架构

下面展示了网关的核心架构设计图：

```
                              ┌─────────────────────────────────────────┐
                              │              GateWay                    │
                              ├─────────────────────────────────────────┤
    ┌──────────┐              │  ┌─────────────────────────────────┐    │
    │  Client  │──HTTP────▶  │  │   Multi-Acceptor (SO_REUSEPORT) │    │
    └──────────┘              │  │   内核级负载均衡                  │    │
         │                    │  └──────────────┬──────────────────┘    │
         │                    │                 │                       │
         │                    │  ┌──────────────▼──────────────────┐    │
         │                    │  │        IO Context Pool          │    │
         │                    │  │     (8 threads, 网络 I/O 池子)   │    │
         │                    │  └──────────────┬──────────────────┘    │
         │                    │                 │                       │
         │                    │  ┌──────────────▼──────────────────┐    │
         │                    │  │           中间件层               │    │
         │                    │  │ [rayelimit] → [request] → [JWT] │    │
         │                    │  └──────────────┬──────────────────┘    │
         │                    │                 │                       │
         │                    │  ┌──────────────▼──────────────────┐    │
         │                    │  │      Business Thread Pool       │    │
         │                    │  │     (8 threads, 业务逻辑池子)     │    │
         │                    │  └──────────────┬──────────────────┘    │
         │                    │                 │                       │
         │                    │  ┌──────────────▼──────────────────┐    │
         │                    │  │ Controller → Service → Repo     │    │
         │                    │  └──────────────┬──────────────────┘    │
         │                    └───────────────────┬─────────────────────┘
         │                                        │
         │          ┌─────────────────────────────┼─────────────────────────────────────┐
         │          │                             │                                     │
         │          ▼                             ▼                                     ▼
         │  ┌───────────────┐             ┌───────────────┐                     ┌───────────────┐
         │  │   MariaDB     │             │    Redis      │                     │  gRPC Client  │
         │  │  (连接池 16)   │             │  (连接池 16)   │                     │ (Channel 池)  │
         │  └───────────────┘             └───────────────┘                     └───────┬───────┘
         │                                                                              │
         │                                                          ┌───────────────────┴───────────────────┐
         │                                                          │                                       │
         │                                                          ▼                                       ▼
         │                                                  ┌──────────────--─┐                       ┌───────────────┐
         │                                                  │ StatusServer    │                       │  VerifyCode   │
         │                                                  │  状态管理服务     │                       │   验证码服务    │
         │                                                  │ (分配ChatServer) │                       │  (发送邮件)    │
         │                                                  └───────┬──────--─┘                       └───────────────┘
         │                                                          │
         │            ┌─────────────────────────────────────────────┘
         │            │ 返回负载最小的 ChatServer 地址 (分布式)
         │            ▼
         │  ┌──────────────────────────────────────────────────────────────────┐
         │  │                     ChatServer 集群                               │
         │  │  ┌─────────────-┐  ┌───────────────┐  ┌─────────────-┐           │
         └--┼▶│ ChatServer1  │  │ ChatServer2   │  │ ChatServer3  │  ...      │
      TCP   │  │  (TCP 长连接) │  │  (TCP 长连接)  │  │  (TCP 长连接) │           │
            │  └─────────────-┘  └────────────---┘  └────────────-─┘           │
            └──────────────────────────────────────────────────────────────────┘
```

### 设计亮点

#### 1. 高性能自研组件

| 组件                   | 描述          | 吞吐量     |
| ---------------------- | ------------- | ---------- |
| **SuperQueue**   | MPMC 无锁队列 | 100M+/s    |
| **Logger**       | 异步日志系统  | 260M+/s    |
| **ID Generator** | 雪花算法/UUID | 40M/23M+/s |

#### 2. 多 Acceptor + SO_REUSEPORT

更新前此 http 服务器使用单 Acceptor 分发连接，容易成为瓶颈，现在选择采用多 Acceptor 架构：

- 每个 `io_context` 线程拥有独立的 Acceptor
- 使用 `SO_REUSEPORT` 允许多个 socket 绑定同一端口
- 由内核进行负载均衡，避免用户态锁竞争

```cpp
// server.cc:27
int reuse_port = 1;
setsockopt(_acceptor.native_handle(), SOL_SOCKET, SO_REUSEPORT, &reuse_port, sizeof(reuse_port));
```

#### 3. 网络层与业务层解耦

IO 线程只负责网络 I/O，业务逻辑投递到独立的线程池执行，处理完成后再回到网络线程发送响应：

```cpp
// 投递到业务线程池
boost::asio::post(Business::GetInstance().GetBusinessPool(), [...] {
    auto result = Logic::GetInstance().HandlePostRequest(ctx);

    // 回到网络线程发送响应
    boost::asio::post(_socket.get_executor(), [...] {
        send_response(self);
    });
});
```

#### 4.. 连接池设计

MariaDB 和 Redis 连接池采用统一的 RAII 设计，确保可以自动回收连接：

- **固定大小数组 + 原子位图**：预分配连接，避免动态内存分配
- **`atomic::wait/notify`**：C++20 原子等待，比条件变量更轻量
- **连接失效自动重连**：通过 `mysql_ping` / `PING` 检测并恢复

```cpp
// db_pool.cc:213 - 无锁获取连接
while (true) {
    for (std::size_t i = 0; i < _pool_size; ++i) {
        bool expected = false;
        if (_slots[i]._in_use.compare_exchange_strong(expected, true, ...)) {
            if (mysql_ping(_slots[i]._conn) != 0) {
                _slots[i]._conn = create_connection();
            }
            return {_slots[i]._conn, this, i};
        }
    }
    _available.wait(0, std::memory_order_relaxed);  // C++20 原子等待
}
```

#### 5. Pimpl 惯用法

所有核心类都采用 Pimpl（编译防火墙）模式，头文件只暴露接口：

- **ABI 稳定**：私有成员变更不影响二进制兼容性
- **编译加速**：减少头文件依赖，降低编译时间
- **实现隐藏**：对外只暴露 `std::unique_ptr<_impl>`

```cpp
// connection.hpp
class Connection : public std::enable_shared_from_this<Connection> {
public:
  explicit Connection(boost::asio::ip::tcp::socket socket);
  void Start();
private:
  struct _impl;
  std::unique_ptr<_impl> _pimpl;  // 隐藏所有实现细节
};
```

#### 6. 完整的中间件链

请求处理遵循责任链模式，层层过滤：

```
Request → [限流检查] → [URL解析] → [请求ID生成] → [JWT校验] → [业务逻辑] → Response
              ↓              ↓              ↓              ↓
           429 拒绝      500 错误      500 错误      401 未授权
```

#### 7. 基于滑动窗口的限流

使用 Redis ZSET 实现滑动窗口限流，每分钟最多 100 次请求：

```cpp
// 清理窗口外的过期记录
conn.Command("ZREMRANGEBYSCORE %s 0 %lld", key.c_str(), now - window_size);
// 添加当前请求
conn.ZAdd(key, now, member);
// 检查数量
if (conn.ZCard(key) > max_requests) return false;
```

每个中间件都可以提前返回，避免无效处理。

#### 8. 安全响应头

自动添加安全防护头，防止常见 Web 攻击：

```cpp
// connection.cc:105
_response.set("X-Frame-Options", "DENY");           // 防止点击劫持
_response.set("X-Content-Type-Options", "nosniff"); // 防止 MIME 嗅探
_response.set("X-Request-ID", request_id);          // 请求追踪
```

#### 9. 安全错误处理

基于 `std::expected` 替代异常，零开销错误传递，以及基于 `std::optional` 实现返回结果的优雅表示：

```cpp
// jwt.hpp:45
[[nodiscard]] static std::optional<TokenPayload> ParseToken(const std::string& token);

// grpc_error.hpp - gRPC 调用返回类型
using VerifyCodeResult = std::expected<VerifyCodeResponse, GrpcError>;

// Id.hpp:47 - ID 生成返回类型
static std::expected<std::string, UuidError> generateUuid() noexcept;
```

#### 10. 优雅的数据库参数绑定

使用变参模板简化 prepared statement 参数绑定：

```cpp
// user_repository.cc
auto params = utils::MakeParams(userId);
auto results = utils::MakeResults(nickname, avatar, email);
conn.QueryOne(sql, params, results);
```

### 模块说明

首先对于 `core` 目录下的 DDD 风格模块进行说明：

| 模块                 | 说明                                             |
| -------------------- | ------------------------------------------------ |
| **Server**     | 多 Acceptor 架构入口，监听端口 10001             |
| **IO**         | io_context 池，Round-Robin 分配连接              |
| **Business**   | 业务线程池，解耦网络与业务                       |
| **Connection** | HTTP 连接对象，处理请求/响应生命周期，选择长连接 |
| **Logic**      | 路由表，注册四大请求处理，分发请求到对应 Handler |
| **Controller** | 控制器层，参数校验与业务编排                     |
| **Service**    | 服务层，核心业务逻辑                             |
| **Repository** | 数据访问层，封装数据库操作                       |
| **Domain**     | 领域模型，DTO/VO/DO 定义                         |

其次是 `utils` 目录下的工具模块：

| 模块                  | 说明                                               |
| --------------------- | -------------------------------------------------- |
| **Common**      | 通用函数，错误码、JWT、限流、全局类型，Argon2id 等 |
| **context**     | 请求上下文，存储请求相关信息                       |
| **DBPool**      | MariaDB 连接池，预分配 + 原子操作                  |
| **RedisPool**   | Redis 连接池，支持全数据结构操作                   |
| **ChannelPool** | gRPC Channel 复用池，给 rpc 客户端使用             |
| **gRPC**        | protobuf 代码生成与 rpc 客户端封装                 |
| **db_params**   | 数据库请求参数配置，实现对增删改查的优雅传参       |
| **URL**         | URL 解析工具，查询参数提取和路径保存               |

其次是 `include` 目录下的全局配置与工具组件：

| 模块             | 说明                        |
| ---------------- | --------------------------- |
| **config** | 读取 CMake 全局变量配置     |
| **global** | 全局配置与无锁队列          |
| **tools**  | 工具组件，ID 生成、Defer 等 |

### API 路由

当前已实现的接口：

| 方法 | 路由                       | 说明       | 需认证 |
| ---- | -------------------------- | ---------- | ------ |
| GET  | `/api/v1/health-check`   | 健康检查   | 否     |
| POST | `/api/v1/user/send-code` | 发送验证码 | 否     |
| POST | `/api/v1/user/register`  | 用户注册   | 否     |
| POST | `/api/v1/user/reset`     | 重置密码   | 否     |
| POST | `/api/v1/user/login`     | 用户登录   | 否     |

### 压测结果

使用 wrk 在本机测试，网关表现稳定，可优雅断开连接和处理容错，在走完完整的请求链后依然可保持 7w+ QPS。

## 开发文档

详细的开发进度和变更记录请参考 [develop.md](./docs/devel/develop.md)。

## TODO

- [ ] HTTPS 支持
