# ChatRoom

## 快速开始

### 初始化仓库

```bash
git clone https://github.com/KBchulan/ChatRoom.git
cd ChatRoom
```

由于 `.git/hooks` 目录不会被提交到远程仓库，因此需要手动安装 Git hooks：

```bash
./scripts/install-hooks.sh
```

这会安装：

- Commit message 格式检查，详细格式请参考开发规范部分。
- CHANGELOG 自动生成

### 配置 LVS 四层负载均衡

### 配置 Nginx 七层负载均衡

### 构建 ChatRoom Client

网关服务相关内容请参考 [客户端指南](chatroom/README.md)。

### 构建 Status Service

状态服务器相关内容请参考 [状态服务器指南](services/StatusServer/README.md)。

### 构建 Verify Service

邮箱验证码服务相关内容请参考 [验证码服务指南](services/VerifyService/README.md)。

### 构建 GateWay Service

网关服务相关内容请参考 [网关指南](services/GateWay/README.md)。

### 构建 Chat Service

聊天服务相关内容请参考 [聊天服务指南](services/ChatService/README.md)。

