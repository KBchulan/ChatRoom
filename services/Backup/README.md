## Backup

该服务是一个简单的备份服务，负责定期备份数据库数据到指定的备份目录，支持全量备份 + 增量备份（binlog）。

### 目录结构

```
.
├── config.yaml   # 配置文件
├── main.go       # 主程序
└── back_data/    # 备份数据目录
```

### 配置说明

```yaml
database:
  host: "127.0.0.1"
  port: 3306
  user: "root"
  password: "xxx"
  name: "chatroom"

backup:
  dir: "./back_data"            # 备份目录
  binlog_dir: "/var/lib/mysql"  # binlog 源目录
  keep_days: 14                 # 备份保留天数
```

### 快速启动

由于 binlog 的权限限制，需要 sudo 权限读取 `/var/lib/mysql` 下的 binlog 文件，因此启动命令需要加上 sudo：

```bash
go mod tidy
sudo go run main.go
```

### 备份策略

| 时间        | 操作                        |
| ----------- | --------------------------- |
| 周日 0 点   | 全量备份 (mysqldump)        |
| 非周日 0 点 | 增量备份 (复制 binlog)      |
| 每次备份后  | 清理超过 keep_days 的旧备份 |

### 恢复方法

可以通过以下步骤恢复数据，即先恢复全量备份，再按顺序应用增量备份：

```bash
# 恢复全量备份
mysql -u root -p chatroom < back_data/full_20241201.sql

# 按顺序应用 binlog，就像下面这样
mysqlbinlog back_data/binlog/20241202/mysql-bin.000001 | mysql -u root -p
```
