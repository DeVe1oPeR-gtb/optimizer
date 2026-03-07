#ifndef OPTIMIZER_PRODUCT_PRODUCT_LOADED_DATA_H
#define OPTIMIZER_PRODUCT_PRODUCT_LOADED_DATA_H

#include <vector>

namespace optimizer {

/// One product's loaded data (measured values, positions, etc.).
/// USERWORK: Layout and fields are product-format-specific.
struct ProductLoadedData {
    std::vector<double> measured;
    std::vector<double> positions;  // optional; same size as measured for 1:1 residual
};

}  // namespace optimizer

#endif  // OPTIMIZER_PRODUCT_PRODUCT_LOADED_DATA_H
