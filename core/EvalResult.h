#ifndef OPTIMIZER_CORE_EVAL_RESULT_H
#define OPTIMIZER_CORE_EVAL_RESULT_H

/**
 * @file EvalResult.h
 * @brief 1 回の目的関数評価の戻り値（スカラ目的値 + 残差ベクトル）
 */

#include <vector>

namespace optimizer {

/**
 * @brief 1回の目的関数評価の結果
 *
 * objective は残差のノルム（例: sqrt(sum(r^2))）など。residuals は全製品の残差を連結したベクトル。
 */
struct EvalResult {
    double objective = 0.0;           /**< 目的関数値（スカラ） */
    std::vector<double> residuals;   /**< 残差ベクトル（全製品連結） */
};

}  // namespace optimizer

#endif  // OPTIMIZER_CORE_EVAL_RESULT_H
