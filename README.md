# ChatRoom

ChatRoom 是一个高性能、高可用的即时通讯系统，支持大规模用户在线聊天。系统采用分布式架构设计，包含负载均衡、网关服务、聊天服务等多个组件，确保系统的稳定性和扩展性。

## 初始化仓库

由于 `.git/hooks` 目录不会被提交到远程仓库，因此需要手动安装 Git hooks，主要包含 commit msg 格式检查和 CHANGELOG 自动生成：

```bash
# 克隆仓库
git clone https://github.com/KBchulan/ChatRoom.git && cd ChatRoom

# 安装 Git hooks
./scripts/install-hooks.sh
```

## 组件指南

| 组件 | 指南 |
| :--- | :--- |
| LVS | [LVS 配置指南](lvs/README.md) |
| Nginx | [Nginx 配置指南](nginx/README.md) |
| 客户端 | [客户端指南](chatroom/README.md) |
| 状态服务器 | [状态服务器指南](services/StatusServer/README.md) |
| 邮箱验证码服务 | [验证码服务指南](services/VerifyCode/README.md) |
| 网关服务 | [网关指南](services/GateWay/README.md) |
| 聊天服务 | [聊天服务指南](services/ChatServer1/README.md) |
| 部署    | [部署指南](docs/deploy.md) |
