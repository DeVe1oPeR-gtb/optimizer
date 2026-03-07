#ifndef OPTIMIZER_PRODUCT_PRODUCT_RESIDUAL_BLOCK_H
#define OPTIMIZER_PRODUCT_PRODUCT_RESIDUAL_BLOCK_H

#include <string>
#include <vector>

namespace optimizer {

/// One product's residual block: offset/size in the concatenated residual vector, and the residuals.
struct ProductResidualBlock {
    std::string product_id;
    size_t offset = 0;
    size_t size = 0;
    std::vector<double> residuals;
    bool ok = true;
    std::string error_message;
};

}  // namespace optimizer

#endif  // OPTIMIZER_PRODUCT_PRODUCT_RESIDUAL_BLOCK_H
