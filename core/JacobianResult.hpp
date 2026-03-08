#ifndef OPTIMIZER_CORE_JACOBIAN_RESULT_H
#define OPTIMIZER_CORE_JACOBIAN_RESULT_H

/**
 * @file JacobianResult.h
 * @brief 残差とヤコビ行列を返す評価の結果（Levenberg–Marquardt 用）
 */

#include <vector>

namespace optimizer {

/**
 * @brief ヤコビアン付き評価の結果（LM 等の勾配法用）
 *
 * jacobian[i][j] = d(residual_i) / d(x_j)。行優先で [残差数][パラメータ数]。
 */
struct JacobianResult {
    std::vector<double> residuals;   /**< 残差ベクトル */
    /** ヤコビ行列 [残差数][パラメータ数]、行優先 */
    std::vector<std::vector<double>> jacobian;
};

}  // namespace optimizer

#endif  // OPTIMIZER_CORE_JACOBIAN_RESULT_H
