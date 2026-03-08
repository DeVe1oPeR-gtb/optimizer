/**
 * @file BatchEvaluationHandler.cpp
 * @brief 製品リストをループし、各製品の残差ブロックを連結して 1 本の残差ベクトルを返す。
 */

#include "product/BatchEvaluationHandler.h"

namespace optimizer {

BatchEvaluationHandler::BatchEvaluationHandler(ProductRunner& runner) : runner_(&runner) {}

void BatchEvaluationHandler::setProducts(std::vector<ProductMeta> products) {
    products_ = std::move(products);
}

ResidualAssembly BatchEvaluationHandler::evaluate(const std::vector<double>& fullParams) {
    ResidualAssembly out;
    size_t offset = 0;
    for (const auto& meta : products_) {
        ProductResidualBlock block = runner_->run(meta, fullParams);
        block.offset = offset;
        if (block.ok)
            offset += block.size;
        else
            block.size = 0;
        out.blocks.push_back(block);
        for (double r : block.residuals)
            out.full_residuals.push_back(r);
    }
    return out;
}

}  // namespace optimizer
