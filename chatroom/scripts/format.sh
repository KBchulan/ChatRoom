#!/usr/bin/env bash
# 格式化 chatroom 目录下的所有 .cc 和 .hpp 文件

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CHATROOM_DIR="$(dirname "$SCRIPT_DIR")"

echo "正在格式化 $CHATROOM_DIR 下的 .cc 和 .hpp 文件..."

find "$CHATROOM_DIR" -type f \( -name "*.cc" -o -name "*.hpp" \) \
    -not -path "*/build/*" \
    -exec clang-format -i {} \;

echo "格式化完成!"