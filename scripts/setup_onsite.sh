#!/usr/bin/env bash
# オンサイト用プロジェクトを、このスクリプトを置いた場所の「親ディレクトリ」をルートとして構築する。
# 毎回、ルート基準で必要なディレクトリを配置し直す。実行後は onsite/main_onsite.cpp を編集し、make onsite でビルド・実行する。
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# プロジェクトルート = setup を置いた場所（scripts の親ディレクトリ）
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$ROOT"

echo "プロジェクトルート: $ROOT"

# 1) ルート基準でディレクトリを配置し直す（毎回実行）
mkdir -p onsite
mkdir -p config
mkdir -p log
mkdir -p result

# 2) main_onsite.cpp が無ければテンプレートからコピー
if [ ! -f onsite/main_onsite.cpp ]; then
    if [ -f "$SCRIPT_DIR/main_onsite.cpp.template" ]; then
        cp "$SCRIPT_DIR/main_onsite.cpp.template" onsite/main_onsite.cpp
        echo "onsite/main_onsite.cpp をテンプレートから作成しました。"
    elif [ -f onsite/main_onsite.cpp ]; then
        :
    else
        echo "error: onsite/main_onsite.cpp がありません。scripts/main_onsite.cpp.template を配置するか、手で作成してください。"
        exit 1
    fi
fi

# 3) Makefile がルートに無い場合は終了（optimizer リポジトリで実行すること）
if [ ! -f Makefile ]; then
    echo "error: ルートに Makefile がありません。この setup は optimizer リポジトリのルート（scripts/ の親）で実行してください。"
    exit 1
fi

# 4) 既に設定済みなら追記しない
if grep -q '^onsite:' Makefile 2>/dev/null; then
    echo "Makefile には既に onsite ターゲットがあります。完了しています。"
    exit 0
fi

# 5) .PHONY に onsite を追加
if sed --version 2>/dev/null | grep -q GNU; then
    sed -i 's/^\.PHONY: all test demo clean$/.PHONY: all test demo clean onsite/' Makefile
else
    sed -i '' 's/^\.PHONY: all test demo clean$/.PHONY: all test demo clean onsite/' Makefile
fi

# 6) フラグメントを Makefile に追記
cat "$SCRIPT_DIR/Makefile.onsite.fragment" >> Makefile

echo "setup_onsite: 完了。main は onsite/main_onsite.cpp を編集し、make onsite でビルド・実行してください。"
