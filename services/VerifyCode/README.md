# ChatRoom VerifyCode

这是 ChatRoom 分布式聊天系统的验证码服务，基于 gRPC 构建的轻量级邮件验证服务，负责生成验证码、发送邮件和存储验证码到 Redis。

## 快速开始

### 环境要求

我是基于原来做的 [模板项目](https://github.com/KBchulan/CMakeTemplate) 进行开发的，除了该模板项目的依赖外，还需要以下环境：

| 依赖      | 版本    | 用途                |
| --------- | ------- | ------------------- |
| CMake     | 3.25+   | 构建系统            |
| GCC/Clang | 13+/16+ | 支持 C++23 的编译器 |
| gRPC      | 1.76+   | RPC 框架            |
| Protobuf  | 33.1+   | 序列化              |
| libcurl   | 8.0+    | SMTP 邮件发送       |
| hiredis   | 1.3+    | Redis 客户端        |
| fmt       | 12.1+   | 格式化输出          |

可以使用自带的包管理器安装，按照标准可以参考 [ci流程](../../.github/workflows/verify-code.yml) 中的安装步骤，该流程在 github actions 中已经通过 ubuntu:latest 的校验。

### 修改配置

可以查看一下 [服务器配置文件](./include/global/Global.hpp)，根据需要修改相关配置项。

**重要**：请将 SMTP 授权码修改为自己的，可在 QQ 邮箱的「设置 → 账号 → POP3/SMTP 服务」中申请，这玩意不要钱。

### 本地开发

本地快速启动验证码服务，可以参考下面的步骤，这个默认启动了 ASan 和 UBSan，会进行内存泄露和未定义行为的检测：

```bash
cmake -S . -B build
cmake --build build

# 确保 Redis 已启动
./build/bin/VerifyCode
```

### 命令行参数

VerifyCode 支持以下命令行参数：

```bash
Usage: VerifyCode [-h] [-p <port>]
Options:
  -h, --help         显示帮助信息
  -p, --port <port>  服务器端口 (默认: 10002)
```

**示例**：

```bash
# 使用默认端口 10002 启动
./build/bin/VerifyCode

# 指定端口 10012 启动
./build/bin/VerifyCode -p 10012
```

更多构建配置可以参考 [指引指南](./docs/guide/README.md)。

## 项目介绍

### 目录说明

下面是本项目的目录结构说明：

```
VerifyCode/
├── src/
│   ├── main.cc                     # 程序入口
│   ├── core/
│   │   ├── server/                 # gRPC 服务实现
│   │   └── smtp/                   # SMTP 邮件发送客户端
│   └── utils/
│       ├── redis/                  # Redis 客户端封装
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

下面展示了验证码服务在整个系统中的位置：

```
                              ┌─────────────────────────────────────────┐
                              │           VerifyCode Server             │
                              │            (gRPC :10002)                │
                              ├─────────────────────────────────────────┤
    ┌──────────┐              │                                         │
    │ GateWay  │───gRPC────▶ │  ┌─────────────────────────────────┐    │
    └──────────┘              │  │     VerifyCodeServiceImpl       │    │
                              │  │                                 │    │
                              │  │  ┌───────────────────────────┐  │    │
                              │  │  │  VerifyCode()             │  │    │
                              │  │  │  - 生成 6 位验证码          │  │    │
                              │  │  │  - 发送 HTML 邮件          │  │    │
                              │  │  │  - 存储到 Redis (60s TTL)  │  │    │
                              │  │  └───────────────────────────┘  │    │
                              │  └─────────────────────────────────┘    │
                              │                 │                       │
                              └─────────────────┼───────────────────────┘
                                                │
                    ┌───────────────────────────┴───────────────────────┐
                    │                                                   │
                    ▼                                                   ▼
            ┌───────────────┐                                   ┌───────────────┐
            │   SmtpClient  │                                   │  RedisClient  │
            │  (libcurl)    │                                   │  (hiredis)    │
            └───────┬───────┘                                   └───────┬───────┘
                    │                                                   │
                    ▼                                                   ▼
            ┌───────────────┐                                   ┌───────────────┐
            │ smtp.qq.com   │                                   │    Redis      │
            │   :465 (SSL)  │                                   │    :6379      │
            └───────────────┘                                   └───────────────┘
```

### RPC 接口定义

VerifyCode 提供一个 gRPC 接口：

```protobuf
service VerifyCodeService {
  // 发送验证码邮件
  rpc VerifyCode(VerifyCodeRequest) returns (VerifyCodeResponse);
}
```

#### VerifyCode

生成验证码并发送到指定邮箱，同时存储到 Redis。

| 字段             | 类型   | 说明                 |
| ---------------- | ------ | -------------------- |
| **请求**   |        |                      |
| email            | string | 目标邮箱地址         |
| **响应**   |        |                      |
| code             | int32  | 状态码，0 表示成功   |
| message          | string | 状态消息             |
| data.verify_code | string | 生成的验证码（6 位） |

### 验证码发送流程

```
┌────────┐         ┌─────────┐         ┌──────────────┐         ┌───────────────┐
│ Client │         │ GateWay │         │ VerifyCode   │         │ QQ SMTP / Redis│
└───┬────┘         └────┬────┘         └──────┬───────┘         └───────┬───────┘
    │                   │                     │                         │
    │   1. 请求验证码     │                     │                         │
    │─────────────────▶│                     │                         │
    │                   │   2. VerifyCode     │                         │
    │                   │───────────────────▶│                         │
    │                   │                     │  3. 生成 6 位验证码       │
    │                   │                     │  (UUID 前 6 位)          │
    │                   │                     │                         │
    │                   │                     │  4. SMTP 发送 HTML 邮件   │
    │                   │                     │───────────────────────▶│
    │                   │                     │◀───────────────────────│
    │                   │                     │                         │
    │                   │                     │  5. Redis SET (TTL=60s) │
    │                   │                     │───────────────────────▶│
    │                   │                     │◀───────────────────────│
    │                   │                     │                         │
    │                   │◀───────────────────│                         │
    │◀─────────────────│   返回验证码          │                         │
```

### 设计亮点

基于最小实现，这个模块没有亮点，就写了个返回的 html 邮件模板，和用 libcurl 发送邮件，还有个优雅退出，谈不上亮点。

### 模块说明

| 模块                            | 说明                              |
| ------------------------------- | --------------------------------- |
| **VerifyCodeServiceImpl** | gRPC 服务实现，处理验证码请求     |
| **SmtpClient**            | SMTP 邮件发送客户端，基于 libcurl |
| **RedisClient**           | Redis 客户端封装，验证码存储      |

### 调用方说明

| 调用方            | 调用接口   | 场景                          |
| ----------------- | ---------- | ----------------------------- |
| **GateWay** | VerifyCode | 用户注册/重置密码时获取验证码 |

## 测试邮件发送

可以使用 curl 直接测试 SMTP 邮件发送功能：

```bash
curl --url "smtp://smtp.qq.com:587" --ssl-reqd \
  --mail-from "your_email@qq.com" \
  --mail-rcpt "target@example.com" \
  --upload-file ./tests/test.txt \
  --user "your_email@qq.com:your_auth_code" \
  --login-options "AUTH=LOGIN"
```

## 开发文档

详细的开发进度和变更记录请参考 [develop.md](./docs/devel/develop.md)。
