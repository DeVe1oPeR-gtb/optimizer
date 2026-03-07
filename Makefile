# Optimizer project: build and tests (no CMake)
CXX = g++
CXXSTD = -std=c++17
CPPUTEST_DIR ?= /home/mcqueen/cpputest
BUILD = build

INC = -I. -IOptimizer -IPSO -ILM -IDE -Iutil -Iparam -Iproduct -Imodel -Iobjective -Icore \
      -I$(CPPUTEST_DIR)/include
CXXFLAGS = $(CXXSTD) -g -Wall $(INC)
LDFLAGS = -L$(CPPUTEST_DIR)/lib -lCppUTest -lCppUTestExt

# Library sources (no tests)
LIB_SRCS = Optimizer/Optimizer.cpp \
           PSO/PSO.cpp LM/LM.cpp DE/DE.cpp \
           param/ParamSpec.cpp param/CsvParamLoader.cpp param/ParameterMapper.cpp \
           model/MockPhysicalModel.cpp model/MockProductDataLoader.cpp \
           product/ProductRunner.cpp product/BatchEvaluationHandler.cpp \
           objective/Objective.cpp

# Test sources
TEST_SRCS = AllTests.cpp PSO/test_PSO.cpp LM/test_LM.cpp DE/test_DE.cpp \
	tests/test_ParameterMapper.cpp tests/test_ProductRunner.cpp \
	tests/test_BatchEvaluationHandler.cpp tests/test_Objective.cpp \
	tests/test_OptimizerConnection.cpp

$(BUILD):
	mkdir -p $(BUILD)

# Per-source rules (avoid path clashes in build/)
$(BUILD)/Optimizer_Optimizer.o: Optimizer/Optimizer.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/PSO_PSO.o: PSO/PSO.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/LM_LM.o: LM/LM.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/DE_DE.o: DE/DE.cpp | $(BUILD)
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
$(BUILD)/PSO_test_PSO.o: PSO/test_PSO.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/LM_test_LM.o: LM/test_LM.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD)/DE_test_DE.o: DE/test_DE.cpp | $(BUILD)
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

all: $(BUILD)/AllTests

test: all
	$(BUILD)/AllTests

clean:
	rm -rf $(BUILD)

.PHONY: all test clean
