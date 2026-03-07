# Optimizer project: build and tests (no CMake)
CXX = g++
CXXSTD = -std=c++17
CPPUTEST_DIR ?= /home/mcqueen/cpputest
BUILD = build

INC = -I. -IOptimizer -Iutil -Iparam -Iproduct -Imodel -Iobjective -Icore \
      -I$(CPPUTEST_DIR)/include
INC_DEMO = -I. -IOptimizer -Iutil -Iparam -Iproduct -Imodel -Iobjective -Icore -Idemo
INC_UTIL = -I. -IOptimizer -Iutil -Iparam -Iproduct -Imodel -Iobjective -Icore
CXXFLAGS = $(CXXSTD) -g -Wall $(INC)
LDFLAGS = -L$(CPPUTEST_DIR)/lib -lCppUTest -lCppUTestExt

# Library sources (no tests)
LIB_SRCS = Optimizer/Optimizer.cpp \
           Optimizer/PSO/PSO.cpp Optimizer/LM/LM.cpp Optimizer/DE/DE.cpp \
           param/ParamSpec.cpp param/CsvParamLoader.cpp param/ParameterMapper.cpp \
           model/MockPhysicalModel.cpp model/MockProductDataLoader.cpp \
           product/ProductRunner.cpp product/BatchEvaluationHandler.cpp \
           objective/Objective.cpp

# Test sources
TEST_SRCS = AllTests.cpp Optimizer/PSO/test_PSO.cpp Optimizer/LM/test_LM.cpp Optimizer/DE/test_DE.cpp \
	tests/test_ParameterMapper.cpp tests/test_ProductRunner.cpp \
	tests/test_BatchEvaluationHandler.cpp tests/test_Objective.cpp \
	tests/test_OptimizerConnection.cpp

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
$(BUILD)/model_MockPhysicalModel.o: model/MockPhysicalModel.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/model_MockProductDataLoader.o: model/MockProductDataLoader.cpp | $(BUILD)
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
# util (TraceConfig, IterationLog)
$(BUILD)/util_TraceConfig.o: util/TraceConfig.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/util_IterationLog.o: util/IterationLog.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
# デモ用（後で削除する前提）
$(BUILD)/demo_DemoPhysicalModel.o: demo/DemoPhysicalModel.cpp | $(BUILD)
	$(CXX) $(CXXSTD) -g -Wall $(INC_DEMO) -c $< -o $@
$(BUILD)/demo_DemoPhysicalModel2.o: demo/DemoPhysicalModel2.cpp | $(BUILD)
	$(CXX) $(CXXSTD) -g -Wall $(INC_DEMO) -c $< -o $@
$(BUILD)/demo_DemoPhysicalModel3.o: demo/DemoPhysicalModel3.cpp | $(BUILD)
	$(CXX) $(CXXSTD) -g -Wall $(INC_DEMO) -c $< -o $@
$(BUILD)/demo_DemoDataLoader.o: demo/DemoDataLoader.cpp | $(BUILD)
	$(CXX) $(CXXSTD) -g -Wall $(INC_DEMO) -c $< -o $@
$(BUILD)/demo_DemoDataLoader2.o: demo/DemoDataLoader2.cpp | $(BUILD)
	$(CXX) $(CXXSTD) -g -Wall $(INC_DEMO) -c $< -o $@
$(BUILD)/demo_DemoDataLoader3.o: demo/DemoDataLoader3.cpp | $(BUILD)
	$(CXX) $(CXXSTD) -g -Wall $(INC_DEMO) -c $< -o $@
$(BUILD)/demo_demo_main.o: demo/demo_main.cpp | $(BUILD)
	$(CXX) $(CXXSTD) -g -Wall $(INC_DEMO) -c $< -o $@

LIB_OBJ_LIST = $(BUILD)/Optimizer_Optimizer.o $(BUILD)/PSO_PSO.o $(BUILD)/LM_LM.o $(BUILD)/DE_DE.o \
	$(BUILD)/param_ParamSpec.o $(BUILD)/param_CsvParamLoader.o $(BUILD)/param_ParameterMapper.o \
	$(BUILD)/model_MockPhysicalModel.o $(BUILD)/model_MockProductDataLoader.o \
	$(BUILD)/product_ProductRunner.o $(BUILD)/product_BatchEvaluationHandler.o \
	$(BUILD)/objective_Objective.o
TEST_OBJ_LIST = $(BUILD)/AllTests.o $(BUILD)/PSO_test_PSO.o $(BUILD)/LM_test_LM.o $(BUILD)/DE_test_DE.o \
	$(BUILD)/tests_test_ParameterMapper.o $(BUILD)/tests_test_ProductRunner.o \
	$(BUILD)/tests_test_BatchEvaluationHandler.o $(BUILD)/tests_test_Objective.o \
	$(BUILD)/tests_test_OptimizerConnection.o

$(BUILD)/AllTests: $(LIB_OBJ_LIST) $(TEST_OBJ_LIST)
	$(CXX) $(CXXSTD) -o $@ $^ $(LDFLAGS)

# デモ実行ファイル（CppUTest 不要。後で削除する前提）
DEMO_OBJ_LIST = $(BUILD)/Optimizer_Optimizer.o $(BUILD)/PSO_PSO.o $(BUILD)/LM_LM.o $(BUILD)/DE_DE.o \
	$(BUILD)/param_ParamSpec.o $(BUILD)/param_CsvParamLoader.o $(BUILD)/param_ParameterMapper.o \
	$(BUILD)/model_MockPhysicalModel.o $(BUILD)/model_MockProductDataLoader.o \
	$(BUILD)/product_ProductRunner.o $(BUILD)/product_BatchEvaluationHandler.o \
	$(BUILD)/objective_Objective.o $(BUILD)/util_TraceConfig.o $(BUILD)/util_IterationLog.o \
	$(BUILD)/demo_DemoPhysicalModel.o $(BUILD)/demo_DemoPhysicalModel2.o $(BUILD)/demo_DemoPhysicalModel3.o \
	$(BUILD)/demo_DemoDataLoader.o $(BUILD)/demo_DemoDataLoader2.o $(BUILD)/demo_DemoDataLoader3.o \
	$(BUILD)/demo_demo_main.o
$(BUILD)/Demo: $(DEMO_OBJ_LIST)
	$(CXX) $(CXXSTD) -o $@ $^

all: $(BUILD)/AllTests

test: all
	$(BUILD)/AllTests

demo: $(BUILD)/Demo
	$(BUILD)/Demo

clean:
	rm -rf $(BUILD)

.PHONY: all test demo clean
