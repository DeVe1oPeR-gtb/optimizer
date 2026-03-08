#!/usr/bin/env bash
# ONSITE と optimizer を統合して para を構築するバッチ。
# 初回: ONSITE_DIR と OPTIMIZER_DIR のフルパスを入力し、integrate/Makefile.para を生成。
# 構築済み: 差分を表示し、OK なら適用する。--diff-only で差分表示のみ（適用しない）。
set -e
DIFF_ONLY=
for arg in "$@"; do
    [ "$arg" = "--diff-only" ] && DIFF_ONLY=1
done

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
CONF="$ROOT/integrate/.onsite_paths"
MAKEFILE_PARA="$ROOT/integrate/Makefile.para"
GENERATOR="$SCRIPT_DIR/generate_integrate_makefile.sh"

cd "$ROOT"
mkdir -p integrate

read_conf() {
    if [ -f "$CONF" ]; then
        # format: ONSITE_DIR=<path>\nOPTIMIZER_DIR=<path> (legacy: NCTC_DIR も読む)
        ONSITE_DIR=$(grep -E '^ONSITE_DIR=' "$CONF" | cut -d= -f2-)
        [ -z "$ONSITE_DIR" ] && ONSITE_DIR=$(grep '^NCTC_DIR=' "$CONF" | cut -d= -f2-)
        OPTIMIZER_DIR=$(grep '^OPTIMIZER_DIR=' "$CONF" | cut -d= -f2-)
        [ -n "$ONSITE_DIR" ] && [ -n "$OPTIMIZER_DIR" ] && return 0
    fi
    return 1
}

write_conf() {
    echo "ONSITE_DIR=$1" > "$CONF"
    echo "OPTIMIZER_DIR=$2" >> "$CONF"
}

generate_makefile() {
    bash "$GENERATOR" "$1" "$2"
}

# --- メイン ---
if read_conf; then
    # 構築済み: diff モード
    echo "構築済みです。ONSITE_DIR=$ONSITE_DIR"
    echo "           OPTIMIZER_DIR=$OPTIMIZER_DIR"
    NEW_MAKEFILE=$(mktemp)
    trap "rm -f '$NEW_MAKEFILE'" EXIT
    generate_makefile "$ONSITE_DIR" "$OPTIMIZER_DIR" > "$NEW_MAKEFILE"

    if [ ! -f "$MAKEFILE_PARA" ]; then
        echo "Makefile.para がありません。新規作成します。"
        cp "$NEW_MAKEFILE" "$MAKEFILE_PARA"
        echo "完了: $MAKEFILE_PARA"
        exit 0
    fi

    if diff -q "$NEW_MAKEFILE" "$MAKEFILE_PARA" >/dev/null 2>&1; then
        echo "差分はありません。"
        exit 0
    fi

    echo "--- 差分 (status) ---"
    diff -u "$MAKEFILE_PARA" "$NEW_MAKEFILE" || true
    echo ""
    if [ -n "$DIFF_ONLY" ]; then
        echo "(--diff-only のため適用しません)"
        exit 0
    fi
    echo "OK - 差分を適用しますか? (y/n)"
    read -r ans
    if [ "$ans" = "y" ] || [ "$ans" = "Y" ]; then
        cp "$NEW_MAKEFILE" "$MAKEFILE_PARA"
        echo "適用しました: $MAKEFILE_PARA"
    else
        echo "適用しませんでした。"
    fi
    exit 0
fi

# 初回: パス入力
echo "ONSITE と optimizer を統合して para を構築します。"
echo "オンサイトの ONSITE ディレクトリのフルパスを入力してください。"
printf "ONSITE_DIR> "
read -r ONSITE_DIR
ONSITE_DIR=$(echo "$ONSITE_DIR" | sed 's/^ *//;s/ *$//')
if [ -z "$ONSITE_DIR" ]; then
    echo "ONSITE_DIR が空です。" >&2
    exit 1
fi
if [ ! -d "$ONSITE_DIR" ]; then
    echo "ディレクトリが存在しません: $ONSITE_DIR" >&2
    exit 1
fi

echo "今回の optimizer ディレクトリのフルパスを入力してください。"
printf "OPTIMIZER_DIR [%s]> " "$ROOT"
read -r OPTIMIZER_DIR
OPTIMIZER_DIR=$(echo "$OPTIMIZER_DIR" | sed 's/^ *//;s/ *$//')
if [ -z "$OPTIMIZER_DIR" ]; then
    OPTIMIZER_DIR="$ROOT"
fi
if [ ! -d "$OPTIMIZER_DIR" ]; then
    echo "ディレクトリが存在しません: $OPTIMIZER_DIR" >&2
    exit 1
fi

# 絶対パスに正規化
ONSITE_DIR=$(cd "$ONSITE_DIR" && pwd)
OPTIMIZER_DIR=$(cd "$OPTIMIZER_DIR" && pwd)

write_conf "$ONSITE_DIR" "$OPTIMIZER_DIR"
generate_makefile "$ONSITE_DIR" "$OPTIMIZER_DIR" > "$MAKEFILE_PARA"
echo "完了しました。"
echo "  ONSITE_DIR=$ONSITE_DIR"
echo "  OPTIMIZER_DIR=$OPTIMIZER_DIR"
echo "  Makefile: $MAKEFILE_PARA"
echo "ビルド: cd integrate && make  # 実行ファイル: para"
