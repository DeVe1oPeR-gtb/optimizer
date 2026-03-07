#ifndef OPTIMIZER_PRODUCT_PRODUCT_RESIDUAL_BLOCK_H
#define OPTIMIZER_PRODUCT_PRODUCT_RESIDUAL_BLOCK_H

#include <string>
#include <vector>

namespace optimizer {

/**
 * @brief 1製品分の残差ブロック
 *
 * 連結残差ベクトル内の offset / size と、その製品の残差列を保持。
 */
struct ProductResidualBlock {
    std::string product_id;         /**< 製品 ID */
    size_t offset = 0;              /**< 連結ベクトル内の開始位置 */
    size_t size = 0;                /**< このブロックの残差個数 */
    std::vector<double> residuals; /**< 残差列 */
    bool ok = true;                 /**< 評価が成功したか */
    std::string error_message;     /**< 失敗時のメッセージ */
};

}  // namespace optimizer

#endif  // OPTIMIZER_PRODUCT_PRODUCT_RESIDUAL_BLOCK_H
