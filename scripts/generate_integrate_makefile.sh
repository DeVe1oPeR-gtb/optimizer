#!/usr/bin/env bash
# Generate integrate/Makefile.para from ONSITE_DIR and OPTIMIZER_DIR.
# Usage: generate_integrate_makefile.sh <ONSITE_DIR> <OPTIMIZER_DIR>
# Output to stdout (caller redirects to integrate/Makefile.para).
set -e
ONSITE_DIR="$1"
OPTIMIZER_DIR="$2"
if [ -z "$ONSITE_DIR" ] || [ -z "$OPTIMIZER_DIR" ]; then
    echo "Usage: $0 <ONSITE_DIR> <OPTIMIZER_DIR>" >&2
    exit 1
fi

cat <<MAKEFILE
# para 統合 Makefile（integrate_project.sh で生成）
# ONSITE と optimizer をリンクして para を構築する。
ONSITE_DIR   := $ONSITE_DIR
OPTIMIZER_DIR := $OPTIMIZER_DIR

CXX         = g++
TARGET      = para
# ONSITE 側で main を持つ .o を除外する場合に設定（例: obj/sim_main.o）
ONSITE_EXCLUDE_OBJ ?=

# ONSITE の obj/*.o（除外指定があれば適用）
ONSITE_OBJS := \$(wildcard \$(ONSITE_DIR)/obj/*.o)
ifneq (\$(ONSITE_EXCLUDE_OBJ),)
ONSITE_OBJS := \$(filter-out \$(ONSITE_DIR)/\$(ONSITE_EXCLUDE_OBJ), \$(ONSITE_OBJS))
endif

OPTIMIZER_OBJS = \\
	\$(OPTIMIZER_DIR)/build/Optimizer_Optimizer.o \\
	\$(OPTIMIZER_DIR)/build/PSO_PSO.o \\
	\$(OPTIMIZER_DIR)/build/LM_LM.o \\
	\$(OPTIMIZER_DIR)/build/DE_DE.o \\
	\$(OPTIMIZER_DIR)/build/param_ParamSpec.o \\
	\$(OPTIMIZER_DIR)/build/param_CsvParamLoader.o \\
	\$(OPTIMIZER_DIR)/build/param_ParameterMapper.o \\
	\$(OPTIMIZER_DIR)/build/product_ProductRunner.o \\
	\$(OPTIMIZER_DIR)/build/product_BatchEvaluationHandler.o \\
	\$(OPTIMIZER_DIR)/build/objective_Objective.o \\
	\$(OPTIMIZER_DIR)/build/util_TraceConfig.o \\
	\$(OPTIMIZER_DIR)/build/util_IterationLog.o \\
	\$(OPTIMIZER_DIR)/build/util_Handler.o \\
	\$(OPTIMIZER_DIR)/build/util_OptimizerDriver.o \\
	\$(OPTIMIZER_DIR)/build/util_LogRotate.o \\
	\$(OPTIMIZER_DIR)/build/util_TerminalMessage.o \\
	\$(OPTIMIZER_DIR)/build/util_DataConfig.o \\
	\$(OPTIMIZER_DIR)/build/util_CoilList.o \\
	\$(OPTIMIZER_DIR)/build/util_CoilDataPath.o \\
	\$(OPTIMIZER_DIR)/build/onsite_main_onsite.o

# ONSITE の LDFLAGS/LIBS に合わせる（必要なら変更）
LDFLAGS = -Wl,--stack,409600000 -static
LIBS    = -lm

all: \$(TARGET)

# ONSITE をその Makefile でビルド（MYHOME を ONSITE_DIR に上書き）
onsite-objs:
	\$(MAKE) -C \$(ONSITE_DIR) MYHOME=\$(ONSITE_DIR)

optimizer-objs:
	\$(MAKE) -C \$(OPTIMIZER_DIR) onsite-objs

\$(TARGET): onsite-objs optimizer-objs
	\$(CXX) \$(LDFLAGS) -o \$(TARGET) \$(ONSITE_OBJS) \$(OPTIMIZER_OBJS) \$(LIBS)
	@echo "built \$(TARGET)"

clean:
	rm -f \$(TARGET)
	\$(MAKE) -C \$(ONSITE_DIR) clean
	\$(MAKE) -C \$(OPTIMIZER_DIR) clean

.PHONY: all onsite-objs optimizer-objs clean
MAKEFILE
