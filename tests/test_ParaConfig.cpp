/**
 * @file test_ParaConfig.cpp
 * @brief ParaConfig のテスト（plog/llog/dlog キー、optimization_data_types、optimization_position）
 */

#include "CppUTest/TestHarness.h"
#include "util/ParaConfig.hpp"
#include <fstream>
#include <cstdio>

TEST_GROUP(ParaConfig_ResultKeys) {
    std::string tmpPath_;
    void setup() override {
        tmpPath_ = "test_para_tmp.cfg";
    }
    void teardown() override {
        std::remove(tmpPath_.c_str());
        optimizer::ParaConfig::resetForTest();
    }
    void writeCfg(const std::string& content) {
        std::ofstream f(tmpPath_);
        f << "optimizer=PSO\n" << content;
        f.close();
    }
};

TEST(ParaConfig_ResultKeys, PlogLlogDlogKeys) {
    writeCfg(
        "plog_filename=result/plog_{timestamp}.csv\n"
        "llog_filename=result/llog.csv\n"
        "dlog_filename=result/dlog_{product_id}.csv\n"
        "detail_enabled=1\n"
        "detail_start_index=2\n"
        "detail_max_points=10\n"
        "llog_one_file=1\n"
    );
    optimizer::ParaConfig::load(tmpPath_);
    STRCMP_EQUAL("result/plog_{timestamp}.csv", optimizer::ParaConfig::getPLOGFilename().c_str());
    STRCMP_EQUAL("result/llog.csv", optimizer::ParaConfig::getLLOGFilename().c_str());
    STRCMP_EQUAL("result/dlog_{product_id}.csv", optimizer::ParaConfig::getDLOGFilename().c_str());
    CHECK(optimizer::ParaConfig::getDetailEnabled());
    LONGS_EQUAL(2, optimizer::ParaConfig::getDetailStartIndex());
    LONGS_EQUAL(10, optimizer::ParaConfig::getDetailMaxPoints());
    CHECK(optimizer::ParaConfig::getLLOGOneFile());
    optimizer::ParaConfig::resetForTest();
}

TEST(ParaConfig_ResultKeys, OptimizationDataTypes) {
    writeCfg("optimization_data_types=A,B,C\n");
    optimizer::ParaConfig::load(tmpPath_);
    const auto& types = optimizer::ParaConfig::getOptimizationDataTypes();
    LONGS_EQUAL(3, types.size());
    STRCMP_EQUAL("A", types[0].c_str());
    STRCMP_EQUAL("B", types[1].c_str());
    STRCMP_EQUAL("C", types[2].c_str());
    CHECK(optimizer::ParaConfig::isDataTypeUsedForOptimization("A"));
    CHECK(optimizer::ParaConfig::isDataTypeUsedForOptimization("B"));
    CHECK(!optimizer::ParaConfig::isDataTypeUsedForOptimization("X"));
    optimizer::ParaConfig::resetForTest();
}

TEST(ParaConfig_ResultKeys, OptimizationPositionRange) {
    writeCfg("optimization_position_min=0.05\noptimization_position_max=0.95\n");
    optimizer::ParaConfig::load(tmpPath_);
    DOUBLES_EQUAL(0.05, optimizer::ParaConfig::getOptimizationPositionMin(), 1e-12);
    DOUBLES_EQUAL(0.95, optimizer::ParaConfig::getOptimizationPositionMax(), 1e-12);
    optimizer::ParaConfig::resetForTest();
}

TEST(ParaConfig_ResultKeys, BackwardCompatOldKeys) {
    writeCfg(
        "result_filename_before=legacy_plog.csv\n"
        "result_detail_filename=legacy_llog.csv\n"
        "result_detail_filename_per_product=legacy_dlog.csv\n"
    );
    optimizer::ParaConfig::load(tmpPath_);
    STRCMP_EQUAL("legacy_plog.csv", optimizer::ParaConfig::getPLOGFilename().c_str());
    STRCMP_EQUAL("legacy_llog.csv", optimizer::ParaConfig::getLLOGFilename().c_str());
    STRCMP_EQUAL("legacy_dlog.csv", optimizer::ParaConfig::getDLOGFilename().c_str());
    optimizer::ParaConfig::resetForTest();
}
