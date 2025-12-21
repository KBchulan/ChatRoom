#!/bin/bash
# ChatRoom LVS 管理脚本

set -e

# ==================== 配置区 ====================
# 工作模式: nat / dr
MODE="nat"

# 虚拟 IP 和端口
VIP="127.0.0.1"
VIP_PORT=8000

# 调度算法: rr(轮询) wrr(加权轮询) lc(最少连接) wlc(加权最少连接)
SCHEDULER="wlc"

# Nginx 后端列表 - IP:PORT:WEIGHT
BACKENDS=(
    "127.0.0.1:9000:1"
)

# DR 模式专用: 网卡名称
INTERFACE="eth0"


# ================== 实际服务 ==================

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

log_info()  { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn()  { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

check_root() {
    if [ "$EUID" -ne 0 ]; then
        log_error "需要 root 权限"
        echo "请使用: sudo $0 $1"
        exit 1
    fi
}

check_ipvsadm() {
    if ! command -v ipvsadm &> /dev/null; then
        log_error "ipvsadm 未安装"
        echo "Arch: sudo pacman -S ipvsadm"
        echo "Ubuntu: sudo apt install ipvsadm"
        exit 1
    fi
}

start_nat() {
    log_info "启动 LVS (NAT 模式)"

    # 开启 IP 转发
    echo 1 > /proc/sys/net/ipv4/ip_forward

    # 清空旧规则
    ipvsadm -C 2>/dev/null || true

    # 添加虚拟服务
    ipvsadm -A -t ${VIP}:${VIP_PORT} -s ${SCHEDULER}

    # 添加后端 -m = NAT/masquerading
    for backend in "${BACKENDS[@]}"; do
        IFS=':' read -r ip port weight <<< "$backend"
        ipvsadm -a -t ${VIP}:${VIP_PORT} -r ${ip}:${port} -m -w ${weight}
    done

    log_info "VIP ${VIP}:${VIP_PORT} -> ${#BACKENDS[@]} backends"
}

start_dr() {
    log_info "启动 LVS (DR 模式)"

    # 绑定 VIP 到网卡
    ip addr add ${VIP}/32 dev ${INTERFACE} 2>/dev/null || true

    # 清空旧规则
    ipvsadm -C 2>/dev/null || true

    # 添加虚拟服务
    ipvsadm -A -t ${VIP}:${VIP_PORT} -s ${SCHEDULER}

    # 添加后端 -g = DR/gatewaying
    for backend in "${BACKENDS[@]}"; do
        IFS=':' read -r ip port weight <<< "$backend"
        ipvsadm -a -t ${VIP}:${VIP_PORT} -r ${ip}:${port} -g -w ${weight}
    done

    log_info "VIP ${VIP}:${VIP_PORT} -> ${#BACKENDS[@]} backends"
    log_warn "DR 模式需要在每个 Nginx 节点运行 realserver.sh"
}

start() {
    check_root "start"
    check_ipvsadm

    case "$MODE" in
        nat) start_nat ;;
        dr)  start_dr ;;
        *)   log_error "未知模式: $MODE"; exit 1 ;;
    esac

    echo ""
    status
}

stop() {
    check_root "stop"

    log_info "停止 LVS"
    ipvsadm -C 2>/dev/null || true

    # DR 模式: 移除 VIP
    if [ "$MODE" = "dr" ]; then
        ip addr del ${VIP}/32 dev ${INTERFACE} 2>/dev/null || true
    fi

    log_info "LVS 已停止"
}

status() {
    echo -e "${CYAN}=== LVS 状态 ===${NC}"
    echo "模式: $MODE | VIP: $VIP:$VIP_PORT | 调度: $SCHEDULER"
    echo ""

    if ! command -v ipvsadm &> /dev/null; then
        log_warn "ipvsadm 未安装"
        return
    fi

    local rules=$(ipvsadm -Ln 2>/dev/null | grep -c "TCP" || echo "0")
    if [ "$rules" -eq "0" ]; then
        log_warn "无活跃规则"
    else
        ipvsadm -Ln --stats 2>/dev/null || ipvsadm -Ln
    fi
}

usage() {
    echo "ChatRoom LVS 管理脚本"
    echo ""
    echo "Usage: sudo $0 {start|stop|restart|status}"
    echo ""
    echo "配置项 (编辑脚本顶部):"
    echo "  MODE       - nat(开发) / dr(生产)"
    echo "  VIP        - 虚拟 IP"
    echo "  VIP_PORT   - 虚拟端口"
    echo "  SCHEDULER  - 调度算法"
    echo "  BACKENDS   - 后端服务器列表"
    exit 1
}

case "${1:-}" in
    start)   start ;;
    stop)    stop ;;
    restart) stop; sleep 1; start ;;
    status)  status ;;
    *)       usage ;;
esac
