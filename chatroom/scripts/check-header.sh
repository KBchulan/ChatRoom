#!/usr/bin/env bash
# 检查所有 .hpp 文件头部是否有 Doxygen 风格的注释（以 /** 开头）

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CHATROOM_DIR="$(dirname "$SCRIPT_DIR")"

missing_header=()

while IFS= read -r -d '' file; do
    first_line=$(head -n 1 "$file")

    if [[ ! "$first_line" =~ ^/\*\* ]]; then
        missing_header+=("$file")
    fi
done < <(find "$CHATROOM_DIR" -type f -name "*.hpp" -not -path "*/build/*" -print0)

if [[ ${#missing_header[@]} -eq 0 ]]; then
    echo "✓ 所有 .hpp 文件都有正确的头部注释"
    exit 0
else
    echo "✗ 以下文件缺少头部注释:"
    for file in "${missing_header[@]}"; do
        echo "  - ${file#$CHATROOM_DIR/}"
    done
    exit 1
fi