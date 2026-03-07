#ifndef OPTIMIZER_PRODUCT_PRODUCT_RUNNER_H
#define OPTIMIZER_PRODUCT_PRODUCT_RUNNER_H

#include "product/ProductMeta.h"
#include "product/ProductLoadedData.h"
#include "product/ProductResidualBlock.h"
#include "model/IPhysicalModel.h"
#include "model/IProductDataLoader.h"
#include <memory>
#include <vector>

namespace optimizer {

/// Runs one product: load data, run physical model, build residual block (measured - predicted).
class ProductRunner {
public:
    ProductRunner(IPhysicalModel& model, IProductDataLoader& loader);

    /// Run for one product. fullParams = full parameter set (same order as ParameterMapper).
    ProductResidualBlock run(const ProductMeta& meta, const std::vector<double>& fullParams);

private:
    IPhysicalModel* model_;
    IProductDataLoader* loader_;
};

}  // namespace optimizer

#endif  // OPTIMIZER_PRODUCT_PRODUCT_RUNNER_H
