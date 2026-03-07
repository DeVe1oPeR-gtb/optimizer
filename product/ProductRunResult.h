#ifndef OPTIMIZER_PRODUCT_PRODUCT_RUN_RESULT_H
#define OPTIMIZER_PRODUCT_PRODUCT_RUN_RESULT_H

#include <string>
#include <vector>

namespace optimizer {

/**
 * @brief 1製品分の計算結果（実測・予測・残差）
 *
 * 適用値のみの計算や最適化終了後の結果出力で、オンサイトの出力関数に渡す。
 */
struct ProductRunResult {
    std::string product_id;
    std::vector<double> measured;   /**< 実測値 */
    std::vector<double> predicted; /**< 予測値（measured と同長） */
    std::vector<double> residuals; /**< 残差（measured - predicted） */
    bool ok = true;
    std::string error_message;
};

}  // namespace optimizer

#endif
