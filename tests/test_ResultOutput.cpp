/**
 * @file test_ResultOutput.cpp
 * @brief ResultOutput / ProductLogBuffer のテスト（PLOG 列追加・product_id マッチ）
 */

#include "CppUTest/TestHarness.h"
#include "util/ProductLogBuffer.hpp"
#include "util/ResultOutput.hpp"
#include "product/ProductRunResult.hpp"
#include <cmath>
#include <vector>

TEST_GROUP(ProductLogBuffer) {};

TEST(ProductLogBuffer, FirstWriteCreatesProductIdAndColumn) {
    optimizer::ProductLogBuffer buf;
    std::vector<optimizer::ProductRunResult> results = {
        {"P1", {1.0, 2.0}, {1.1, 2.1}, {0.1, 0.1}, true, "", {}},
        {"P2", {3.0}, {2.9}, {0.1}, true, "", {}},
    };
    buf.write(results, "rmse_apply");
    LONGS_EQUAL(2, buf.getHeaders().size());
    STRCMP_EQUAL("product_id", buf.getHeaders()[0].c_str());
    STRCMP_EQUAL("rmse_apply", buf.getHeaders()[1].c_str());
    LONGS_EQUAL(2, buf.getRows().size());
    STRCMP_EQUAL("P1", buf.getRows()[0][0].c_str());
    STRCMP_EQUAL("P2", buf.getRows()[1][0].c_str());
    DOUBLES_EQUAL(std::sqrt(0.01), std::stod(buf.getRows()[0][1]), 1e-9);
    DOUBLES_EQUAL(0.1, std::stod(buf.getRows()[1][1]), 1e-9);
}

TEST(ProductLogBuffer, SecondWriteAppendsColumnAndMatchesProductId) {
    optimizer::ProductLogBuffer buf;
    std::vector<optimizer::ProductRunResult> r1 = {
        {"A", {1.0}, {1.0}, {0.0}, true, "", {}},
        {"B", {2.0}, {2.0}, {0.0}, true, "", {}},
    };
    buf.write(r1, "col1");
    std::vector<optimizer::ProductRunResult> r2 = {
        {"A", {1.0}, {0.0}, {1.0}, true, "", {}},   // rmse 1.0
        {"B", {2.0}, {1.0}, {1.0}, true, "", {}},   // rmse 1.0
    };
    buf.write(r2, "col2");
    LONGS_EQUAL(3, buf.getHeaders().size());
    STRCMP_EQUAL("col2", buf.getHeaders()[2].c_str());
    LONGS_EQUAL(2, buf.getRows().size());
    LONGS_EQUAL(3, buf.getRows()[0].size());
    LONGS_EQUAL(3, buf.getRows()[1].size());
    DOUBLES_EQUAL(1.0, std::stod(buf.getRows()[0][2]), 1e-9);  // A col2
    DOUBLES_EQUAL(1.0, std::stod(buf.getRows()[1][2]), 1e-9);  // B col2
}

TEST(ProductLogBuffer, ClearEmptiesBuffer) {
    optimizer::ProductLogBuffer buf;
    std::vector<optimizer::ProductRunResult> r = {
        {"X", {1.0}, {1.0}, {0.0}, true, "", {}},
    };
    buf.write(r, "x");
    CHECK(!buf.empty());
    buf.clear();
    CHECK(buf.empty());
    LONGS_EQUAL(0, buf.getHeaders().size());
    LONGS_EQUAL(0, buf.getRows().size());
}

TEST_GROUP(ResultOutput_PLOG) {};

TEST(ResultOutput_PLOG, WritePLOGDelegatesToBuffer) {
    optimizer::ResultOutput ro;
    ro.setPLOGFilename("out.csv");
    std::vector<optimizer::ProductRunResult> results = {
        {"id1", {1.0, 2.0}, {1.0, 2.0}, {0.0, 0.0}, true, "", {}},
    };
    ro.writePLOG(results, "rmse");
    ro.flushPLOG();  // ファイルに書き出し（パスは replacePlaceholders で "out.csv" のまま）
    // 再度 write するとバッファは空なのでエラーにならない
    ro.writePLOG(results, "rmse2");
    CHECK(!ro.lastFlushSkipped());
}
