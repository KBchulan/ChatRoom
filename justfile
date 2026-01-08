# ChatRoom 服务管理脚本
# 使用方法: just <command> [service]

# 默认命令
default:
    @just --list

# ============== 配置 ==============

# 服务目录映射
services_dir := "services"
client_dir := "chatroom"

# PID 文件目录
pid_dir := ".pids"

# C++ 服务列表: 别名 -> 目录名
cpp_services := "gateway:GateWay status:StatusServer verify:VerifyCode chat:ChatServer"

# go 服务列表: 别名 -> 目录名
go_services := "backup:Backup"

# ============== 帮助 ==============

# 显示详细帮助
help:
    @echo "ChatRoom 服务管理工具"
    @echo ""
    @echo "服务类型:"
    @echo "  客户端: client"
    @echo "  Go 服务: backup"
    @echo "  C++ 服务: gateway, status, verify, chat"
    @echo ""
    @echo "编译命令:"
    @echo "  just build all                       编译所有服务"
    @echo "  just build [service]                 编译指定服务"
    @echo "  just clean all                       清理构建目录"
    @echo "  just clean [service]                 清理指定服务构建目录"
    @echo ""
    @echo "运行命令:"
    @echo "  just run client                      运行客户端"
    @echo "  just run all                         启动所有后端服务"
    @echo "  just run [service]                   启动指定服务，使用默认端口"
    @echo "  just run [service] <port>            启动指定服务，使用指定端口"
    @echo "  just stop all                        停止所有服务"
    @echo "  just stop [service]                  停止指定服务"
    @echo "  just restart all                     重启所有服务"
    @echo "  just restart [service]               重启指定服务"
    @echo ""
    @echo "状态与日志:"
    @echo "  just status                          查看服务状态"
    @echo "  just logs [service]                  查看指定服务日志"
    @echo "  just logs [service] <line>           查看具体行数的指定服务日志"
    @echo "  just logs-follow [services]          实时跟踪指定服务日志"
    @echo ""
    @echo "开发命令:"
    @echo "  just dev client                      编译客户端并前台启动"
    @echo "  just dev [service]                   编译指定服务并前台启动"
    @echo "  just dev [service] <port>            编译指定服务并前台启动，使用指定端口"
    @echo ""
    @echo "流水线模式:"
    @echo "  just workflows all                   启动所有服务的流水线"
    @echo "  just workflows [service]             启动指定服务的流水线"
    @echo ""


# ============== 编译命令 ==============

# 编译服务: all/gateway/status/verify/chat/backup/client
build service="all":
    #!/usr/bin/env bash
    set -euo pipefail

    mkdir -p {{pid_dir}}

    build_cpp() {
        local name=$1
        local dir=$2
        echo "编译 $name..."
        (
            cd {{services_dir}}/$dir
            cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
            cmake --build build -j$(nproc)
        )
        echo "$name 编译完成"
    }

    build_go() {
        echo "编译 Backup..."
        (
            cd {{services_dir}}/Backup
            go build -o backup main.go
        )
        echo "Backup 编译完成"
    }

    build_client() {
        echo "编译 Client..."
        (
            cd {{client_dir}}
            cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
            cmake --build build -j$(nproc)
        )
        echo "Client 编译完成"
    }

    case "{{service}}" in
        all)
            build_cpp "GateWay" "GateWay"
            build_cpp "StatusServer" "StatusServer"
            build_cpp "VerifyCode" "VerifyCode"
            build_cpp "ChatServer" "ChatServer"
            build_go
            build_client
            echo "所有服务编译完成！"
            ;;
        gateway)  build_cpp "GateWay" "GateWay" ;;
        status)   build_cpp "StatusServer" "StatusServer" ;;
        verify)   build_cpp "VerifyCode" "VerifyCode" ;;
        chat)     build_cpp "ChatServer" "ChatServer" ;;
        backup)   build_go ;;
        client)   build_client ;;
        *)
            echo "未知服务: {{service}}"
            echo "可用服务: all, gateway, status, verify, chat, backup, client"
            exit 1
            ;;
    esac

# 清理构建目录: all/gateway/status/verify/chat/backup/client
clean service="all":
    #!/usr/bin/env bash
    set -euo pipefail

    clean_dir() {
        local name=$1
        local dir=$2
        if [ -d "$dir/build" ]; then
            echo "清理 $name..."
            rm -rf "$dir/build"
            echo "$name 清理完成"
        fi
    }

    case "{{service}}" in
        all)
            clean_dir "GateWay" "{{services_dir}}/GateWay"
            clean_dir "StatusServer" "{{services_dir}}/StatusServer"
            clean_dir "VerifyCode" "{{services_dir}}/VerifyCode"
            clean_dir "ChatServer" "{{services_dir}}/ChatServer"
            clean_dir "Client" "{{client_dir}}"
            rm -f {{services_dir}}/Backup/backup
            rm -rf {{pid_dir}}
            echo "全部构建目录清理完成"
            ;;
        gateway)  clean_dir "GateWay" "{{services_dir}}/GateWay" ;;
        status)   clean_dir "StatusServer" "{{services_dir}}/StatusServer" ;;
        verify)   clean_dir "VerifyCode" "{{services_dir}}/VerifyCode" ;;
        chat)     clean_dir "ChatServer" "{{services_dir}}/ChatServer" ;;
        backup)   rm -f {{services_dir}}/Backup/backup ;;
        client)   clean_dir "Client" "{{client_dir}}" ;;
        *)
            echo "未知服务: {{service}}"
            exit 1
            ;;
    esac

# ============== 运行命令 ==============

# 启动服务在后台运行
run service port="":
    #!/usr/bin/env bash
    set -euo pipefail

    mkdir -p {{pid_dir}}

    run_cpp() {
        local name=$1
        local dir=$2
        local bin=$3
        local default_port=$4
        local port=${5:-$default_port}
        local pid_file="{{pid_dir}}/${name}.pid"

        if [ -f "$pid_file" ] && kill -0 $(cat "$pid_file") 2>/dev/null; then
            echo "$name 已在运行 (PID: $(cat $pid_file))"
            return
        fi

        local bin_path="{{services_dir}}/$dir/build/bin/$bin"
        if [ ! -f "$bin_path" ]; then
            echo "$name 未编译，请先运行: just build ${name,,}"
            exit 1
        fi

        echo "启动 $name (端口: $port)..."
        nohup "$bin_path" -p "$port" > {{pid_dir}}/${name}.log 2>&1 &
        echo $! > "$pid_file"
        echo "$name 已启动 (PID: $!)"
    }

    run_go() {
        local root_dir="$(pwd)"
        local pid_file="$root_dir/{{pid_dir}}/backup.pid"
        local log_file="$root_dir/{{pid_dir}}/backup.log"

        if [ -f "$pid_file" ] && kill -0 $(cat "$pid_file") 2>/dev/null; then
            echo "Backup 已在运行 (PID: $(cat $pid_file))"
            return
        fi

        if [ ! -f "{{services_dir}}/Backup/backup" ]; then
            echo "Backup 未编译，请先运行: just build backup"
            exit 1
        fi

        echo "启动 Backup..."
        (
            cd {{services_dir}}/Backup
            nohup ./backup > "$log_file" 2>&1 &
            echo $! > "$pid_file"
            echo "Backup 已启动 (PID: $!)"
        )
    }

    run_client() {
        local bin_path="{{client_dir}}/build/chatroom"
        if [ ! -f "$bin_path" ]; then
            echo "Client 未编译，请先运行: just build client"
            exit 1
        fi
        echo "启动 Client..."
        "$bin_path"
    }

    case "{{service}}" in
        gateway)  run_cpp "GateWay" "GateWay" "GateWay" "10001" "{{port}}" ;;
        verify)   run_cpp "VerifyCode" "VerifyCode" "VerifyCode" "10002" "{{port}}" ;;
        status)   run_cpp "StatusServer" "StatusServer" "StatusServer" "10003" "{{port}}" ;;
        chat)     run_cpp "ChatServer" "ChatServer" "ChatServer" "10004" "{{port}}" ;;
        backup)   run_go ;;
        client)   run_client ;;
        all)
            # 按依赖顺序启动
            just run verify
            just run status
            just run gateway
            just run chat
            just run backup
            echo "所有服务已启动！"
            ;;
        *)
            echo "未知服务: {{service}}"
            echo "可用服务: all, gateway, status, verify, chat, backup, client"
            exit 1
            ;;
    esac

# 停止服务
stop service="all":
    #!/usr/bin/env bash
    set -euo pipefail

    stop_service() {
        local name=$1
        local pid_file="{{pid_dir}}/${name}.pid"

        if [ -f "$pid_file" ]; then
            local pid=$(cat "$pid_file")
            if kill -0 "$pid" 2>/dev/null; then
                echo "停止 $name (PID: $pid)..."
                kill "$pid"
                rm -f "$pid_file"
                echo "$name 已停止"
            else
                echo "$name 已经停止"
                rm -f "$pid_file"
            fi
        else
            echo "$name 未运行"
        fi
    }

    case "{{service}}" in
        all)
            stop_service "ChatServer"
            stop_service "GateWay"
            stop_service "StatusServer"
            stop_service "VerifyCode"
            stop_service "backup"
            echo "所有服务已停止"
            ;;
        gateway)  stop_service "GateWay" ;;
        status)   stop_service "StatusServer" ;;
        verify)   stop_service "VerifyCode" ;;
        chat)     stop_service "ChatServer" ;;
        backup)   stop_service "backup" ;;
        *)
            echo "未知服务: {{service}}"
            exit 1
            ;;
    esac

# 重启服务
restart service:
    just stop {{service}}
    @sleep 1
    just run {{service}}

# ============== 状态与日志 ==============

# 查看服务状态
status:
    #!/usr/bin/env bash
    echo "服务状态"
    echo "─────────────────────────────────────"
    printf "%-15s %-8s %-10s\n" "服务" "状态" "PID"
    echo "─────────────────────────────────────"

    check_status() {
        local name=$1
        local pid_file="{{pid_dir}}/${name}.pid"

        if [ -f "$pid_file" ]; then
            local pid=$(cat "$pid_file")
            if kill -0 "$pid" 2>/dev/null; then
                printf "%-15s \033[32m%-8s\033[0m %-10s\n" "$name" "运行中" "$pid"
            else
                printf "%-15s \033[31m%-8s\033[0m %-10s\n" "$name" "已停止" "-"
            fi
        else
            printf "%-15s \033[33m%-8s\033[0m %-10s\n" "$name" "未启动" "-"
        fi
    }

    check_status "GateWay"
    check_status "StatusServer"
    check_status "VerifyCode"
    check_status "ChatServer"
    check_status "backup"
    echo "─────────────────────────────────────"

# 查看服务日志
logs service lines="50":
    #!/usr/bin/env bash
    case "{{service}}" in
        gateway)  log_file="GateWay.log" ;;
        status)   log_file="StatusServer.log" ;;
        verify)   log_file="VerifyCode.log" ;;
        chat)     log_file="ChatServer.log" ;;
        backup)   log_file="backup.log" ;;
        *)
            echo "未知服务: {{service}}"
            exit 1
            ;;
    esac

    if [ -f "{{pid_dir}}/$log_file" ]; then
        tail -n {{lines}} "{{pid_dir}}/$log_file"
    else
        echo "日志文件不存在"
    fi

# 实时查看日志
logs-follow service:
    #!/usr/bin/env bash
    case "{{service}}" in
        gateway)  log_file="GateWay.log" ;;
        status)   log_file="StatusServer.log" ;;
        verify)   log_file="VerifyCode.log" ;;
        chat)     log_file="ChatServer.log" ;;
        backup)   log_file="backup.log" ;;
        *)
            echo "未知服务: {{service}}"
            exit 1
            ;;
    esac

    if [ -f "{{pid_dir}}/$log_file" ]; then
        tail -f "{{pid_dir}}/$log_file"
    else
        echo "日志文件不存在"
    fi

# ============== 开发命令 ==============

# 开发模式: 编译并前台启动单个服务
dev service port="":
    #!/usr/bin/env bash
    set -euo pipefail

    just build {{service}}

    case "{{service}}" in
        gateway)
            port="{{port}}"
            [ -z "$port" ] && port="10001"
            echo "启动 GateWay (端口: $port, 前台模式)..."
            {{services_dir}}/GateWay/build/bin/GateWay -p "$port"
            ;;
        verify)
            port="{{port}}"
            [ -z "$port" ] && port="10002"
            echo "启动 VerifyCode (端口: $port, 前台模式)..."
            {{services_dir}}/VerifyCode/build/bin/VerifyCode -p "$port"
            ;;
        status)
            port="{{port}}"
            [ -z "$port" ] && port="10003"
            echo "启动 StatusServer (端口: $port, 前台模式)..."
            {{services_dir}}/StatusServer/build/bin/StatusServer -p "$port"
            ;;
        chat)
            port="{{port}}"
            [ -z "$port" ] && port="10004"
            echo "启动 ChatServer (端口: $port, 前台模式)..."
            {{services_dir}}/ChatServer/build/bin/ChatServer -p "$port"
            ;;
        backup)
            echo "启动 Backup (前台模式)..."
            cd {{services_dir}}/Backup && ./backup
            ;;
        client)
            echo "启动 Client..."
            {{client_dir}}/build/chatroom
            ;;
        *)
            echo "未知服务: {{service}}"
            exit 1
            ;;
    esac

# ============== 流水线命令 ==============

# 流水线模式: 启动某个服务的流水线
workflows service="all":
    #!/usr/bin/env bash
    set -euo pipefail

    start_workflow() {
        local name=$1
        local dir="{{services_dir}}/$name"
        echo "启动 $name 流水线..."
        (
            cd "$dir"
            ./scripts/workflow.sh
        )
        echo "$name 流水线完成"
    }

    case "{{service}}" in
        all)
            start_workflow "ChatServer"
            start_workflow "GateWay"
            start_workflow "StatusServer"
            start_workflow "VerifyCode"
            echo "所有服务流水线已完成！"
            ;;
        gateway)  start_workflow "GateWay" ;;
        status)   start_workflow "StatusServer" ;;
        verify)   start_workflow "VerifyCode" ;;
        chat)     start_workflow "ChatServer" ;;
        *)
            echo "未知服务: {{service}}"
            exit 1
            ;;
    esac
