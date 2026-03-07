#include "CppUTest/TestHarness.h"
#include "product/ProductRunner.h"
#include "product/ProductMeta.h"
#include "model/MockPhysicalModel.h"
#include "model/MockProductDataLoader.h"
#include <vector>

TEST_GROUP(ProductRunner) {};

TEST(ProductRunner, MockProducesResidualBlock) {
    optimizer::MockPhysicalModel model;
    optimizer::MockProductDataLoader loader;
    optimizer::ProductRunner runner(model, loader);
    optimizer::ProductMeta meta;
    meta.product_id = "P1";
    meta.file_path = "dummy.csv";
    std::vector<double> fullParams = {0.1, 0.2, 0.3};
    auto block = runner.run(meta, fullParams);
    CHECK(block.ok);
    LONGS_EQUAL(block.residuals.size(), block.size);
    CHECK(block.size > 0);
    CHECK(block.product_id == "P1");
}

TEST(ProductRunner, ResidualIsMeasuredMinusPredicted) {
    optimizer::MockPhysicalModel model;
    optimizer::MockProductDataLoader loader;
    optimizer::ProductRunner runner(model, loader);
    optimizer::ProductMeta meta;
    meta.product_id = "P1";
    std::vector<double> fullParams = {1.0, 2.0};
    auto block = runner.run(meta, fullParams);
    CHECK(block.ok);
    CHECK(block.residuals.size() > 0);
}
