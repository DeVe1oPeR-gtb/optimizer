#ifndef OPTIMIZER_PRODUCT_RESIDUAL_ASSEMBLY_H
#define OPTIMIZER_PRODUCT_RESIDUAL_ASSEMBLY_H

#include "product/ProductResidualBlock.h"
#include <vector>

namespace optimizer {

/**
 * @brief 全製品の残差を連結したベクトルと、製品ごとのブロック情報
 */
struct ResidualAssembly {
    std::vector<double> full_residuals;              /**< 全ブロックを連結した残差ベクトル */
    std::vector<ProductResidualBlock> blocks;        /**< 製品ごとのブロック（offset/size 付き） */
};

}  // namespace optimizer

#endif  // OPTIMIZER_PRODUCT_RESIDUAL_ASSEMBLY_H
