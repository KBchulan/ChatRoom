# ChatRoom Nginx

便携式 Nginx 反向代理配置，所有文件都在当前目录内运行。

## 架构

```
Client -> LVS(:8000) -> Nginx集群(:9000) -> Gateway集群(:10001)
```

## 文件说明

| 文件           | 说明           |
| -------------- | -------------- |
| `nginx.conf` | Nginx 配置文件 |
| `nginx.sh`   | 管理脚本       |

## 快速开始

根据实际启动的 Gateway 实例，编辑 `nginx.conf` 中的 upstream 块添加更多 Gateway，随后用下面的命令启动 Nginx 服务：

```bash
./nginx.sh start
```

## 脚本命令

```bash
./nginx.sh start    # 启动
./nginx.sh stop     # 停止
./nginx.sh restart  # 重启
./nginx.sh reload   # 重载配置，不中断服务
./nginx.sh status   # 查看状态
./nginx.sh test     # 测试配置语法
```

## 日志格式

访问日志位于 `logs/access.log`，包含以下字段：

- `rt` - 请求总耗时
- `uct` - upstream 连接时间
- `urt` - upstream 响应时间
