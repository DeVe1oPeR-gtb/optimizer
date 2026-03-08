#!/usr/bin/env bash
# 現場ルートから src/sim をコピーし、optimizer の必要な部分だけをコピーして
# 新しいオンサイト用プロジェクトを1ディレクトリに構築する。
#
# 使い方:
#   bash scripts/setup_new_onsite_project.sh --field-root /path/to/field_project --target /path/to/new_project
#   bash scripts/setup_new_onsite_project.sh --field-root /path/to/field_project --target /path/to/new_project --optimizer-dir /path/to/optimizer
#
# --field-root  既存のオンサイト物理モデルプロジェクト（src/ と sim/ がある）
# --target      新規作成するプロジェクトのパス（存在しなければ作成）
# --optimizer-dir  optimizer リポジトリのパス（省略時はこのスクリプトの親ディレクトリ）
#
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OPTIMIZER_DIR_DEFAULT="$(cd "$SCRIPT_DIR/.." && pwd)"
FIELD_ROOT=""
TARGET=""
OPTIMIZER_DIR="$OPTIMIZER_DIR_DEFAULT"

while [ $# -gt 0 ]; do
    case "$1" in
        --field-root)   FIELD_ROOT="$2"; shift 2 ;;
        --target)       TARGET="$2";     shift 2 ;;
        --optimizer-dir) OPTIMIZER_DIR="$2"; shift 2 ;;
        -h|--help)
            echo "Usage: $0 --field-root DIR --target DIR [--optimizer-dir DIR]"
            echo "  --field-root   Existing onsite project containing src/ and sim/"
            echo "  --target       New project directory to create"
            echo "  --optimizer-dir  Optimizer repo path (default: script parent)"
            exit 0
            ;;
        *) echo "Unknown option: $1" >&2; exit 1 ;;
    esac
done

if [ -z "$FIELD_ROOT" ] || [ -z "$TARGET" ]; then
    echo "Error: --field-root and --target are required." >&2
    echo "  Example: $0 --field-root /path/to/field --target /path/to/new_project" >&2
    exit 1
fi

FIELD_ROOT="$(cd "$FIELD_ROOT" && pwd)"
OPTIMIZER_DIR="$(cd "$OPTIMIZER_DIR" && pwd)"
mkdir -p "$TARGET"
TARGET="$(cd "$TARGET" && pwd)"

echo "Field root (src/sim source): $FIELD_ROOT"
echo "New project target:         $TARGET"
echo "Optimizer source:           $OPTIMIZER_DIR"

# 1) src と sim を現場ルートからコピー
for dir in src sim; do
    if [ -d "$FIELD_ROOT/$dir" ]; then
        echo "Copying $dir/ from field root..."
        cp -r "$FIELD_ROOT/$dir" "$TARGET/$dir"
    else
        echo "Warning: $FIELD_ROOT/$dir not found; creating empty $dir/."
        mkdir -p "$TARGET/$dir"
    fi
done

# 2) optimizer から必要なディレクトリのみコピー（mock/tests は含めない）
OPT_COPY_DIRS=(Optimizer param product objective model core util onsite config)
for d in "${OPT_COPY_DIRS[@]}"; do
    if [ -d "$OPTIMIZER_DIR/$d" ]; then
        echo "Copying optimizer $d/..."
        rm -rf "$TARGET/$d"
        cp -r "$OPTIMIZER_DIR/$d" "$TARGET/$d"
    else
        echo "Warning: $OPTIMIZER_DIR/$d not found; skipping."
    fi
done

# 3) ビルド・ログ用ディレクトリ
mkdir -p "$TARGET/build" "$TARGET/log" "$TARGET/result"

# 4) スタンドアロン用 Makefile を生成（onsite 用のみ、mock なし・LogRotate 含む）
INC="-I. -IOptimizer -Iutil -Iparam -Iproduct -Imodel -Iobjective -Icore -Ionsite -Isrc -Isim"
CXXFLAGS="-std=c++17 -g -Wall $INC"

cat > "$TARGET/Makefile" << 'MAKEFILE_HEAD'
# オンサイト用スタンドアロンビルド（setup_new_onsite_project.sh で生成）
# mock/tests は含まない。現場の src/sim は -Isrc -Isim で参照。
CXX = g++
CXXSTD = -std=c++17
BUILD = build
INC = -I. -IOptimizer -Iutil -Iparam -Iproduct -Imodel -Iobjective -Icore -Ionsite -Isrc -Isim
CXXFLAGS = $(CXXSTD) -g -Wall $(INC)

$(BUILD):
	mkdir -p $(BUILD)

# Optimizer
$(BUILD)/Optimizer_Optimizer.o: Optimizer/Optimizer.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/PSO_PSO.o: Optimizer/PSO/PSO.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/LM_LM.o: Optimizer/LM/LM.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/DE_DE.o: Optimizer/DE/DE.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# param
$(BUILD)/param_ParamSpec.o: param/ParamSpec.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/param_CsvParamLoader.o: param/CsvParamLoader.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/param_ParameterMapper.o: param/ParameterMapper.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# product
$(BUILD)/product_ProductRunner.o: product/ProductRunner.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/product_BatchEvaluationHandler.o: product/BatchEvaluationHandler.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# objective
$(BUILD)/objective_Objective.o: objective/Objective.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# util
$(BUILD)/util_TraceConfig.o: util/TraceConfig.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_IterationLog.o: util/IterationLog.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_Handler.o: util/Handler.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_OptimizerDriver.o: util/OptimizerDriver.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_LogRotate.o: util/LogRotate.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_TerminalMessage.o: util/TerminalMessage.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_DataConfig.o: util/DataConfig.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_CoilList.o: util/CoilList.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_CoilDataPath.o: util/CoilDataPath.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# onsite main
$(BUILD)/onsite_main_onsite.o: onsite/main_onsite.cpp | $(BUILD)
	$(CXX) $(CXXSTD) -g -Wall $(INC) -Ionsite -c $< -o $@

ONSITE_OBJ_LIST = $(BUILD)/Optimizer_Optimizer.o $(BUILD)/PSO_PSO.o $(BUILD)/LM_LM.o $(BUILD)/DE_DE.o \
	$(BUILD)/param_ParamSpec.o $(BUILD)/param_CsvParamLoader.o $(BUILD)/param_ParameterMapper.o \
	$(BUILD)/product_ProductRunner.o $(BUILD)/product_BatchEvaluationHandler.o \
	$(BUILD)/objective_Objective.o $(BUILD)/util_TraceConfig.o $(BUILD)/util_IterationLog.o \
	$(BUILD)/util_Handler.o $(BUILD)/util_OptimizerDriver.o $(BUILD)/util_LogRotate.o \
	$(BUILD)/util_TerminalMessage.o $(BUILD)/util_DataConfig.o $(BUILD)/util_CoilList.o $(BUILD)/util_CoilDataPath.o \
	$(BUILD)/onsite_main_onsite.o

$(BUILD)/Onsite: $(ONSITE_OBJ_LIST)
	$(CXX) $(CXXSTD) -o $@ $^

all: $(BUILD)/Onsite
onsite: all
	@echo "--- run Onsite ---"
	$(BUILD)/Onsite

clean:
	rm -rf $(BUILD)

.PHONY: all onsite clean
MAKEFILE_HEAD

echo ""
echo "Done. New project layout:"
echo "  $TARGET/"
echo "  ├── src/        (from field root)"
echo "  ├── sim/        (from field root)"
echo "  ├── Optimizer/  (optimizer, no tests)"
echo "  ├── param/ product/ objective/ model/ core/ util/ onsite/ config/"
echo "  ├── build/ log/ result/"
echo "  └── Makefile"
echo ""
echo "Next: cd $TARGET && make onsite"
echo "Edit: onsite/main_onsite.cpp (and link to sim/src as needed)."
