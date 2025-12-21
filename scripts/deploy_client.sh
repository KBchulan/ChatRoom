#!/bin/bash
# ChatRoom 客户端 AppImage 打包脚本

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
CLIENT_DIR="$PROJECT_ROOT/chatroom"
BUILD_DIR="$CLIENT_DIR/build"
WORK_DIR="$BUILD_DIR/appimage"
OUTPUT_DIR="$PROJECT_ROOT/deploy/client"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

info() { echo -e "${GREEN}[INFO]${NC} $1"; }
warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
error() { echo -e "${RED}[ERROR]${NC} $1"; exit 1; }

# 检查构建产物
if [[ ! -f "$BUILD_DIR/chatroom" ]]; then
    error "未找到可执行文件, 请先构建: cd chatroom && cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build"
fi

info "开始打包 AppImage..."

# 创建目录
rm -rf "$WORK_DIR"
mkdir -p "$WORK_DIR" "$OUTPUT_DIR"
cd "$WORK_DIR"

# 下载 linuxdeploy
LINUXDEPLOY="$BUILD_DIR/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_QT="$BUILD_DIR/linuxdeploy-plugin-qt-x86_64.AppImage"

if [[ ! -f "$LINUXDEPLOY" ]]; then
    info "下载 linuxdeploy..."
    wget -q -O "$LINUXDEPLOY" https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
    chmod +x "$LINUXDEPLOY"
fi

if [[ ! -f "$LINUXDEPLOY_QT" ]]; then
    info "下载 linuxdeploy-plugin-qt..."
    wget -q -O "$LINUXDEPLOY_QT" https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
    chmod +x "$LINUXDEPLOY_QT"
fi

# 创建 .desktop 文件
cat > chatroom.desktop << EOF
[Desktop Entry]
Type=Application
Name=ChatRoom
Exec=chatroom
Icon=chatroom
Categories=Network;
EOF

info "正在打包..."

# 禁用 strip
export NO_STRIP=1

# 指定 Qt6 的 qmake
export QMAKE=/usr/bin/qmake6

# 构建 linuxdeploy 命令
DEPLOY_CMD=(
    "$LINUXDEPLOY"
    --appdir AppDir
    --executable "$BUILD_DIR/chatroom"
    --desktop-file chatroom.desktop
    --icon-file "$CLIENT_DIR/resources/chatroom.png"
    --plugin qt
)

# 添加 SSL 库用于 HTTPS 支持
SSL_LIBS=("/usr/lib/libssl.so.3" "/usr/lib/libcrypto.so.3")
for lib in "${SSL_LIBS[@]}"; do
    if [[ -f "$lib" ]]; then
        DEPLOY_CMD+=(--library "$lib")
        info "添加 SSL 库: $(basename "$lib")"
    else
        warn "未找到 SSL 库: $lib"
    fi
done

DEPLOY_CMD+=(--output appimage)
"${DEPLOY_CMD[@]}"

# 移动到输出目录
mv ChatRoom-*.AppImage "$OUTPUT_DIR/ChatRoom-x86_64.AppImage"

# 清理工作目录
rm -rf "$WORK_DIR"

info "打包完成！"
echo ""
echo "输出文件: $OUTPUT_DIR/ChatRoom-x86_64.AppImage"
