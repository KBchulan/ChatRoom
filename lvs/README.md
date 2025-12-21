# ChatRoom LVS

这是本项目的 Linux Virtual Server 四层负载均衡配置。

## 架构

```
Client -> LVS(:8000) -> Nginx集群(:9000) -> Gateway集群(:10001)
```

## 文件说明

| 文件              | 说明                               |
| ----------------- | ---------------------------------- |
| `lvs.sh`        | LVS 主控脚本，在负载均衡节点运行   |
| `nginx-node.sh` | DR 模式专用，在每个 Nginx 节点运行 |

## 快速开始

在本机开发时只需要用到 [lvs](./lvs.sh) 脚本，这里先介绍单机情况下的使用策略。

### 安装 ipvsadm

```bash
# Arch
sudo pacman -S ipvsadm

# Ubuntu/Debian
sudo apt install ipvsadm
```

### 更新配置

编辑 `lvs.sh` 顶部的配置区，其中提供了几个变量:

- `MODE`: 工作模式，可选择 `nat` 或 `dr`，建议本机开发时使用 `nat` 模式，生产环境使用 `dr` 模式
- `VIP` 和 `VIP_PORT`: 虚拟 IP 和端口，客户端请求的入口
- `SCHEDULER`: 调度算法，可选 `rr`、`wrr`、`lc`、`wlc`
- `BACKENDS`: Nginx 后端列表，格式为 `IP:PORT:WEIGHT`，可添加多个后端

### 快速启动

```bash
sudo ./lvs.sh start
```

## 脚本命令

该脚本还提供了一些服务，如下:

```bash
sudo ./lvs.sh start     # 启动
sudo ./lvs.sh stop      # 停止
sudo ./lvs.sh restart   # 重启
sudo ./lvs.sh status    # 查看状态
```

## 部署

如果需要部署到云端，此时需要把模式修改为 `dr`，并在任何一个 `nginx` 的服务器运行 `nginx-node.sh`。
