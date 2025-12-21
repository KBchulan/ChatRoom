## 集群化部署

对于每个组件的部署，各个目录下都有文档了，可以参考对应目录下的 `README.md` 文件，目前我的硬件情况下单机可以达到 7w+ QPS，如果追求更高的性能，可以通过集群化部署来提升整体的吞吐量。

> 这个部分指的是对 Http 服务相关的扩容，Tcp 层级有其他优化策略。

## 示例

首先假设你最初有三台服务器，分别运行了 LVS、Nginx 和 GateWay 服务，接下来我们通过增加 GateWay 和 Nginx 实例来提升整体的吞吐量。

### 增加 GateWay 实例

首先可以部署多台 GateWay 服务，这部分可以参考 [网关指南](../services/GateWay/README.md)，此时我们就获得到了多台 GateWay 实例。

接着可以修改 Nginx 服务器的 [配置文件](../nginx/nginx.conf)，在 `upstream` 模块中增加新的 GateWay 实例地址，例如：

```nginx
upstream gateway_cluster {
  least_conn;

  # 增加更多 Gateway 实例
  server 1.2.3.4:10001 weight=1 max_fails=3 fail_timeout=30s;
  server 1.2.3.5:10001 weight=1 max_fails=3 fail_timeout=30s;
  server 1.2.3.6:10001 weight=1 max_fails=3 fail_timeout=30s;
  server 1.2.3.7:10001 weight=1 max_fails=3 fail_timeout=30s;

  keepalive 64;
}
```

随后重启 Nginx 服务，即可将请求分发到更多的 GateWay 实例上，从而提升整体的吞吐量，此部分的提高上限取决于 Nginx 服务器的带宽和性能，预期可以达到 20w+ QPS。

### 增加 Nginx 实例

如果需要更多的吞吐量，可以继续增加 Nginx 实例，这部分可以参考 [Nginx 配置指南](../nginx/README.md)，假设我们又增加了三台 Nginx 服务器。

接着需要修改 LVS 服务器的 [配置文件](../lvs/lvs.sh)，在最上面的 Backends 列表中增加新的 Nginx 实例地址，例如：

```bash
BACKENDS=(
  "2.2.3.4:9000:1"
  "2.2.3.5:9000:1"
  "2.2.3.6:9000:1"
  "2.2.3.7:9000:1"
)
```

随后在每个 Nginx 模块运行 [连接脚本](../lvs/nginx-node.sh)，即可连接到 lvs 服务，预计可达到 100w+ QPS。
