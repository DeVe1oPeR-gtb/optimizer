/**
 * @file test_TraceConfig.cpp
 * @brief TraceConfig のテスト（plog/llog/dlog キー、optimization_data_types、optimization_position）
 */

#include "CppUTest/TestHarness.h"
#include "util/TraceConfig.hpp"
#include <fstream>
#include <cstdio>

TEST_GROUP(TraceConfig_ResultKeys) {
    std::string tmpPath_;
    void setup() override {
        tmpPath_ = "test_para_tmp.cfg";
    }
    void teardown() override {
        std::remove(tmpPath_.c_str());
    }
    void writeCfg(const std::string& content) {
        std::ofstream f(tmpPath_);
        f << "optimizer=PSO\n" << content;
        f.close();
    }
};

TEST(TraceConfig_ResultKeys, PlogLlogDlogKeys) {
    IGNORE_ALL_LEAKS_IN_TEST();  // TraceConfig::load は静的変数に代入するため
    writeCfg(
        "plog_filename=result/plog_{timestamp}.csv\n"
        "llog_filename=result/llog.csv\n"
        "dlog_filename=result/dlog_{product_id}.csv\n"
        "detail_enabled=1\n"
        "detail_start_index=2\n"
        "detail_max_points=10\n"
        "llog_one_file=1\n"
    );
    optimizer::TraceConfig::load(tmpPath_);
    STRCMP_EQUAL("result/plog_{timestamp}.csv", optimizer::TraceConfig::getPLOGFilename().c_str());
    STRCMP_EQUAL("result/llog.csv", optimizer::TraceConfig::getLLOGFilename().c_str());
    STRCMP_EQUAL("result/dlog_{product_id}.csv", optimizer::TraceConfig::getDLOGFilename().c_str());
    CHECK(optimizer::TraceConfig::getDetailEnabled());
    LONGS_EQUAL(2, optimizer::TraceConfig::getDetailStartIndex());
    LONGS_EQUAL(10, optimizer::TraceConfig::getDetailMaxPoints());
    CHECK(optimizer::TraceConfig::getLLOGOneFile());
}

TEST(TraceConfig_ResultKeys, OptimizationDataTypes) {
    IGNORE_ALL_LEAKS_IN_TEST();
    writeCfg("optimization_data_types=A,B,C\n");
    optimizer::TraceConfig::load(tmpPath_);
    const auto& types = optimizer::TraceConfig::getOptimizationDataTypes();
    LONGS_EQUAL(3, types.size());
    STRCMP_EQUAL("A", types[0].c_str());
    STRCMP_EQUAL("B", types[1].c_str());
    STRCMP_EQUAL("C", types[2].c_str());
    CHECK(optimizer::TraceConfig::isDataTypeUsedForOptimization("A"));
    CHECK(optimizer::TraceConfig::isDataTypeUsedForOptimization("B"));
    CHECK(!optimizer::TraceConfig::isDataTypeUsedForOptimization("X"));
}

TEST(TraceConfig_ResultKeys, OptimizationPositionRange) {
    IGNORE_ALL_LEAKS_IN_TEST();
    writeCfg("optimization_position_min=0.05\noptimization_position_max=0.95\n");
    optimizer::TraceConfig::load(tmpPath_);
    DOUBLES_EQUAL(0.05, optimizer::TraceConfig::getOptimizationPositionMin(), 1e-12);
    DOUBLES_EQUAL(0.95, optimizer::TraceConfig::getOptimizationPositionMax(), 1e-12);
}

TEST(TraceConfig_ResultKeys, BackwardCompatOldKeys) {
    IGNORE_ALL_LEAKS_IN_TEST();
    writeCfg(
        "result_filename_before=legacy_plog.csv\n"
        "result_detail_filename=legacy_llog.csv\n"
        "result_detail_filename_per_product=legacy_dlog.csv\n"
    );
    optimizer::TraceConfig::load(tmpPath_);
    STRCMP_EQUAL("legacy_plog.csv", optimizer::TraceConfig::getPLOGFilename().c_str());
    STRCMP_EQUAL("legacy_llog.csv", optimizer::TraceConfig::getLLOGFilename().c_str());
    STRCMP_EQUAL("legacy_dlog.csv", optimizer::TraceConfig::getDLOGFilename().c_str());
}
