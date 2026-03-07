#ifndef OPTIMIZER_PRODUCT_RESIDUAL_ASSEMBLY_H
#define OPTIMIZER_PRODUCT_RESIDUAL_ASSEMBLY_H

#include "product/ProductResidualBlock.h"
#include <vector>

namespace optimizer {

/// Full concatenated residuals and per-product blocks (with offset/size).
struct ResidualAssembly {
    std::vector<double> full_residuals;
    std::vector<ProductResidualBlock> blocks;
};

}  // namespace optimizer

#endif  // OPTIMIZER_PRODUCT_RESIDUAL_ASSEMBLY_H
