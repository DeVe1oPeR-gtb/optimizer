# Optimizer project: build and tests (no CMake)
CXX = g++
CXXSTD = -std=c++17
CPPUTEST_DIR ?= /home/mcqueen/cpputest
BUILD = build

INC = -I. -IOptimizer -Iutil -Iparam -Iproduct -Imodel -Iobjective -Icore -Imock \
      -I$(CPPUTEST_DIR)/include
INC_DEMO = -I. -IOptimizer -Iutil -Iparam -Iproduct -Imodel -Iobjective -Icore -Imock
INC_UTIL = -I. -IOptimizer -Iutil -Iparam -Iproduct -Imodel -Iobjective -Icore
CXXFLAGS = $(CXXSTD) -g -Wall $(INC)
LDFLAGS = -L$(CPPUTEST_DIR)/lib -lCppUTest -lCppUTestExt

# Library sources (no tests)
LIB_SRCS = Optimizer/Optimizer.cpp \
           Optimizer/PSO/PSO.cpp Optimizer/LM/LM.cpp Optimizer/DE/DE.cpp \
           param/ParamSpec.cpp param/CsvParamLoader.cpp param/ParameterMapper.cpp \
           mock/Mock.cpp \
           product/ProductRunner.cpp product/BatchEvaluationHandler.cpp \
           objective/Objective.cpp

# Test sources
TEST_SRCS = AllTests.cpp Optimizer/PSO/test_PSO.cpp Optimizer/LM/test_LM.cpp Optimizer/DE/test_DE.cpp \
	tests/test_ParameterMapper.cpp tests/test_ProductRunner.cpp \
	tests/test_BatchEvaluationHandler.cpp tests/test_Objective.cpp \
	tests/test_OptimizerConnection.cpp tests/test_ResultOutput.cpp tests/test_ParaConfig.cpp

$(BUILD):
	mkdir -p $(BUILD)

# Per-source rules (avoid path clashes in build/)
$(BUILD)/Optimizer_Optimizer.o: Optimizer/Optimizer.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/PSO_PSO.o: Optimizer/PSO/PSO.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/LM_LM.o: Optimizer/LM/LM.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/DE_DE.o: Optimizer/DE/DE.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/param_ParamSpec.o: param/ParamSpec.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/param_CsvParamLoader.o: param/CsvParamLoader.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/param_ParameterMapper.o: param/ParameterMapper.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/mock_Mock.o: mock/Mock.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/product_ProductRunner.o: product/ProductRunner.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/product_BatchEvaluationHandler.o: product/BatchEvaluationHandler.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/objective_Objective.o: objective/Objective.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/AllTests.o: AllTests.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/PSO_test_PSO.o: Optimizer/PSO/test_PSO.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/LM_test_LM.o: Optimizer/LM/test_LM.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/DE_test_DE.o: Optimizer/DE/test_DE.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/tests_test_ParameterMapper.o: tests/test_ParameterMapper.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/tests_test_ProductRunner.o: tests/test_ProductRunner.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/tests_test_BatchEvaluationHandler.o: tests/test_BatchEvaluationHandler.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/tests_test_Objective.o: tests/test_Objective.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/tests_test_OptimizerConnection.o: tests/test_OptimizerConnection.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
# util (ParaConfig, IterationLog)
$(BUILD)/util_ParaConfig.o: util/ParaConfig.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_IterationLog.o: util/IterationLog.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
# util (Handler, OptimizerDriver)
$(BUILD)/util_Handler.o: util/Handler.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_OptimizerDriver.o: util/OptimizerDriver.cpp | $(BUILD)
	$(CXX) $(CXXSTD) -g -Wall $(INC_DEMO) -c $< -o $@
# util (TerminalMessage, DataConfig, CoilList, CoilDataPath)
$(BUILD)/util_TerminalMessage.o: util/TerminalMessage.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_DataConfig.o: util/DataConfig.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_CoilList.o: util/CoilList.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_CoilDataPath.o: util/CoilDataPath.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_LogRotate.o: util/LogRotate.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_ProductLogBuffer.o: util/ProductLogBuffer.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_ResultOutput.o: util/ResultOutput.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
# mock（テスト用モック + デモ用モデル・ローダ・エントリ）
$(BUILD)/mock_Demo.o: mock/Demo.cpp | $(BUILD)
	$(CXX) $(CXXSTD) -g -Wall $(INC_DEMO) -c $< -o $@
$(BUILD)/mock_demo_main.o: mock/demo_main.cpp | $(BUILD)
	$(CXX) $(CXXSTD) -g -Wall $(INC_DEMO) -c $< -o $@

LIB_OBJ_LIST = $(BUILD)/Optimizer_Optimizer.o $(BUILD)/PSO_PSO.o $(BUILD)/LM_LM.o $(BUILD)/DE_DE.o \
	$(BUILD)/param_ParamSpec.o $(BUILD)/param_CsvParamLoader.o $(BUILD)/param_ParameterMapper.o \
	$(BUILD)/mock_Mock.o \
	$(BUILD)/product_ProductRunner.o $(BUILD)/product_BatchEvaluationHandler.o \
	$(BUILD)/objective_Objective.o
TEST_OBJ_LIST = $(BUILD)/AllTests.o $(BUILD)/PSO_test_PSO.o $(BUILD)/LM_test_LM.o $(BUILD)/DE_test_DE.o \
	$(BUILD)/tests_test_ParameterMapper.o $(BUILD)/tests_test_ProductRunner.o \
	$(BUILD)/tests_test_BatchEvaluationHandler.o $(BUILD)/tests_test_Objective.o \
	$(BUILD)/tests_test_OptimizerConnection.o $(BUILD)/tests_test_ResultOutput.o $(BUILD)/tests_test_ParaConfig.o
# util のうちテストで参照するオブジェクト（ParaConfig, ResultOutput, ProductLogBuffer, TerminalMessage）
TEST_UTIL_OBJ = $(BUILD)/util_ParaConfig.o $(BUILD)/util_ProductLogBuffer.o $(BUILD)/util_ResultOutput.o $(BUILD)/util_TerminalMessage.o

$(BUILD)/tests_test_ResultOutput.o: tests/test_ResultOutput.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/tests_test_ParaConfig.o: tests/test_ParaConfig.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/AllTests: $(LIB_OBJ_LIST) $(TEST_OBJ_LIST) $(TEST_UTIL_OBJ)
	$(CXX) $(CXXSTD) -o $@ $^ $(LDFLAGS)

# デモ実行ファイル（CppUTest 不要。後で削除する前提）
DEMO_OBJ_LIST = $(BUILD)/Optimizer_Optimizer.o $(BUILD)/PSO_PSO.o $(BUILD)/LM_LM.o $(BUILD)/DE_DE.o \
	$(BUILD)/param_ParamSpec.o $(BUILD)/param_CsvParamLoader.o $(BUILD)/param_ParameterMapper.o \
	$(BUILD)/product_ProductRunner.o $(BUILD)/product_BatchEvaluationHandler.o \
	$(BUILD)/objective_Objective.o $(BUILD)/util_ParaConfig.o $(BUILD)/util_IterationLog.o \
	$(BUILD)/util_Handler.o $(BUILD)/util_OptimizerDriver.o \
	$(BUILD)/util_TerminalMessage.o $(BUILD)/util_DataConfig.o $(BUILD)/util_CoilList.o $(BUILD)/util_CoilDataPath.o \
	$(BUILD)/util_LogRotate.o $(BUILD)/util_ProductLogBuffer.o $(BUILD)/util_ResultOutput.o $(BUILD)/mock_Demo.o $(BUILD)/mock_demo_main.o
$(BUILD)/Demo: $(DEMO_OBJ_LIST)
	$(CXX) $(CXXSTD) -o $@ $^

all: $(BUILD)/AllTests

test: all
	$(BUILD)/AllTests

demo: $(BUILD)/Demo
	$(BUILD)/Demo

clean:
	rm -rf $(BUILD)

.PHONY: all test demo clean onsite
# --- Onsite（scripts/setup_onsite.sh で Makefile に追記）---
$(BUILD)/onsite_main_onsite.o: onsite/main_onsite.cpp | $(BUILD)
	$(CXX) $(CXXSTD) -g -Wall $(INC_DEMO) -Ionsite -c $< -o $@

ONSITE_OBJ_LIST = $(BUILD)/Optimizer_Optimizer.o $(BUILD)/PSO_PSO.o $(BUILD)/LM_LM.o $(BUILD)/DE_DE.o \
	$(BUILD)/param_ParamSpec.o $(BUILD)/param_CsvParamLoader.o $(BUILD)/param_ParameterMapper.o \
	$(BUILD)/product_ProductRunner.o $(BUILD)/product_BatchEvaluationHandler.o \
	$(BUILD)/objective_Objective.o $(BUILD)/util_ParaConfig.o $(BUILD)/util_IterationLog.o \
	$(BUILD)/util_Handler.o $(BUILD)/util_OptimizerDriver.o $(BUILD)/util_LogRotate.o \
	$(BUILD)/util_TerminalMessage.o $(BUILD)/util_DataConfig.o $(BUILD)/util_CoilList.o $(BUILD)/util_CoilDataPath.o \
	$(BUILD)/util_ProductLogBuffer.o $(BUILD)/util_ResultOutput.o $(BUILD)/onsite_main_onsite.o
$(BUILD)/Onsite: $(ONSITE_OBJ_LIST)
	$(CXX) $(CXXSTD) -o $@ $^

onsite: $(BUILD)/Onsite
	@echo "--- run Onsite ---"
	$(BUILD)/Onsite

# 統合ビルド用: .o のみビルド（integrate_project.sh から利用）
onsite-objs: $(ONSITE_OBJ_LIST)
.PHONY: onsite-objs
