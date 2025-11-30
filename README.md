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

### 构建 ChatRoom Client

网关服务相关内容请参考 [客户端指南](chatroom/README.md)。

### 构建 GateWay Service

网关服务相关内容请参考 [网关指南](services/GateWay/docs/guide/README.md)。

## 开发规范

### Commit Message 格式

本项目使用 [Conventional Commits](https://www.conventionalcommits.org/) 规范：

```
<type>[(scope)]: <description>
```

**支持的类型**:

- `feat`: 新功能
- `fix`: Bug 修复
- `docs`: 文档更新
- `style`: 代码格式
- `refactor`: 重构
- `test`: 测试
- `chore`: 杂项

> 不符合格式的 commit 会被 Git hook 拒绝

### 提交自测

对于前端部分，我们并没有做过多要求，但是对于任何一个服务，在提交代码前，请在本地运行 `scripts/workflow.sh` 文件进行自测，确保所有测试通过且代码覆盖率符合要求，这个脚本在每个服务的根目录。
