#include "CppUTest/TestHarness.h"
#include "product/BatchEvaluationHandler.h"
#include "product/ProductRunner.h"
#include "product/ProductMeta.h"
#include "model/MockPhysicalModel.h"
#include "model/MockProductDataLoader.h"
#include <vector>

TEST_GROUP(BatchEvaluationHandler) {};

TEST(BatchEvaluationHandler, ConcatenateTwoProducts) {
    optimizer::MockPhysicalModel model;
    optimizer::MockProductDataLoader loader;
    optimizer::ProductRunner runner(model, loader);
    optimizer::BatchEvaluationHandler batch(runner);
    std::vector<optimizer::ProductMeta> products = {
        {"P1", "f1"},
        {"P2", "f2"},
    };
    batch.setProducts(products);
    std::vector<double> fullParams = {0.5, 0.5, 0.5};
    auto asm_ = batch.evaluate(fullParams);
    LONGS_EQUAL(2, asm_.blocks.size());
    size_t total = 0;
    for (const auto& b : asm_.blocks) {
        total += b.size;
        CHECK(b.offset + b.size <= asm_.full_residuals.size());
    }
    LONGS_EQUAL(total, asm_.full_residuals.size());
}

TEST(BatchEvaluationHandler, OffsetSizeConsistent) {
    optimizer::MockPhysicalModel model;
    optimizer::MockProductDataLoader loader;
    optimizer::ProductRunner runner(model, loader);
    optimizer::BatchEvaluationHandler batch(runner);
    batch.setProducts({{"A", "a"}, {"B", "b"}});
    auto asm_ = batch.evaluate({1.0, 1.0});
    size_t pos = 0;
    for (const auto& b : asm_.blocks) {
        LONGS_EQUAL(pos, b.offset);
        LONGS_EQUAL(b.residuals.size(), b.size);
        for (size_t i = 0; i < b.size; ++i)
            DOUBLES_EQUAL(b.residuals[i], asm_.full_residuals[b.offset + i], 1e-12);
        pos += b.size;
    }
}
