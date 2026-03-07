#ifndef OPTIMIZER_MODEL_I_PRODUCT_DATA_LOADER_H
#define OPTIMIZER_MODEL_I_PRODUCT_DATA_LOADER_H

#include "product/ProductLoadedData.h"
#include "product/ProductMeta.h"
#include <memory>

namespace optimizer {

/// Load one product's data from path (or id). USERWORK: Replace with real file/DB load.
class IProductDataLoader {
public:
    virtual ~IProductDataLoader() = default;

    /// Load data for the given product. Returns nullptr on failure.
    virtual std::unique_ptr<ProductLoadedData> load(const ProductMeta& meta) = 0;
};

}  // namespace optimizer

#endif  // OPTIMIZER_MODEL_I_PRODUCT_DATA_LOADER_H
