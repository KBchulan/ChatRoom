#!/bin/bash
# DR 模式: Nginx 节点配置脚本

set -e

# ==================== 配置区 ====================
# 必须和 lvs.sh 中的 VIP 一致
VIP="127.0.0.1"
# ================================================

start() {
    echo "[INFO] 配置 RealServer (DR 模式)"

    # 绑定 VIP 到 loopback
    ip addr add ${VIP}/32 dev lo 2>/dev/null || true

    # 禁止 ARP 响应
    echo 1 > /proc/sys/net/ipv4/conf/lo/arp_ignore
    echo 2 > /proc/sys/net/ipv4/conf/lo/arp_announce
    echo 1 > /proc/sys/net/ipv4/conf/all/arp_ignore
    echo 2 > /proc/sys/net/ipv4/conf/all/arp_announce

    echo "[INFO] VIP $VIP 已绑定到 lo"
}

stop() {
    echo "[INFO] 移除 RealServer 配置"

    ip addr del ${VIP}/32 dev lo 2>/dev/null || true

    echo 0 > /proc/sys/net/ipv4/conf/lo/arp_ignore
    echo 0 > /proc/sys/net/ipv4/conf/lo/arp_announce
    echo 0 > /proc/sys/net/ipv4/conf/all/arp_ignore
    echo 0 > /proc/sys/net/ipv4/conf/all/arp_announce

    echo "[INFO] 配置已清除"
}

case "${1:-}" in
    start) start ;;
    stop)  stop ;;
    *)
        echo "Usage: sudo $0 {start|stop}"
        echo "在每个 Nginx 节点运行此脚本 (DR 模式专用)"
        exit 1
        ;;
esac
