#!/bin/bash
# ChatRoom Nginx 管理脚本

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
NGINX_CONF="${SCRIPT_DIR}/nginx.conf"
NGINX_PID="${SCRIPT_DIR}/logs/nginx.pid"

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
CYAN='\033[0;36m'
NC='\033[0m'

log_info()  { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn()  { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

init_dirs() {
    mkdir -p "${SCRIPT_DIR}/logs"
    mkdir -p "${SCRIPT_DIR}/temp/client_body"
    mkdir -p "${SCRIPT_DIR}/temp/proxy"
    mkdir -p "${SCRIPT_DIR}/temp/fastcgi"
    mkdir -p "${SCRIPT_DIR}/temp/uwsgi"
    mkdir -p "${SCRIPT_DIR}/temp/scgi"
}

is_running() {
    [ -f "$NGINX_PID" ] && kill -0 "$(cat "$NGINX_PID")" 2>/dev/null
}

start() {
    if is_running; then
        log_warn "Nginx 已在运行 (PID: $(cat "$NGINX_PID"))"
        return 1
    fi

    init_dirs
    log_info "启动 Nginx"
    nginx -p "$SCRIPT_DIR" -c "$NGINX_CONF"

    sleep 0.5
    if is_running; then
        log_info "Nginx 启动成功 (PID: $(cat "$NGINX_PID"))"
    else
        log_error "Nginx 启动失败，请检查日志: ${SCRIPT_DIR}/logs/error.log"
        return 1
    fi
}

stop() {
    if ! is_running; then
        log_warn "Nginx 未运行"
        return 0
    fi

    log_info "停止 Nginx"
    nginx -p "$SCRIPT_DIR" -c "$NGINX_CONF" -s stop
    log_info "已停止"
}

reload() {
    if ! is_running; then
        log_warn "Nginx 未运行，尝试启动"
        start
        return $?
    fi

    log_info "重载配置"
    nginx -p "$SCRIPT_DIR" -c "$NGINX_CONF" -s reload
    log_info "配置已重载"
}

status() {
    echo -e "${CYAN}=== Nginx 状态 ===${NC}"

    if is_running; then
        log_info "运行中 (PID: $(cat "$NGINX_PID"))"
        echo ""
        echo "Worker 进程:"
        ps aux | grep "[n]ginx: worker" | head -5
    else
        log_warn "未运行"
    fi
}

test_conf() {
    log_info "测试配置"
    nginx -p "$SCRIPT_DIR" -c "$NGINX_CONF" -t
}

case "${1:-}" in
    start)   start ;;
    stop)    stop ;;
    restart) stop; sleep 1; start ;;
    reload)  reload ;;
    status)  status ;;
    test)    test_conf ;;
    *)
        echo "Usage: $0 {start|stop|restart|reload|status|test}"
        exit 1
        ;;
esac
