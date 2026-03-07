#ifndef OPTIMIZER_PRODUCT_BATCH_EVALUATION_HANDLER_H
#define OPTIMIZER_PRODUCT_BATCH_EVALUATION_HANDLER_H

#include "product/ProductMeta.h"
#include "product/ResidualAssembly.h"
#include "product/ProductRunner.h"
#include <vector>

namespace optimizer {

/// Algorithm-agnostic: loops over products, runs ProductRunner for each, concatenates residuals.
class BatchEvaluationHandler {
public:
    explicit BatchEvaluationHandler(ProductRunner& runner);

    /// Set product list (meta only; data is loaded per run).
    void setProducts(std::vector<ProductMeta> products);

    /// Evaluate all products with given full parameter set. Returns concatenated residuals and blocks.
    ResidualAssembly evaluate(const std::vector<double>& fullParams);

private:
    ProductRunner* runner_;
    std::vector<ProductMeta> products_;
};

}  // namespace optimizer

#endif  // OPTIMIZER_PRODUCT_BATCH_EVALUATION_HANDLER_H
