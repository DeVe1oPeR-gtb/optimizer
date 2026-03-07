#ifndef OPTIMIZER_CORE_EVAL_RESULT_H
#define OPTIMIZER_CORE_EVAL_RESULT_H

#include <vector>

namespace optimizer {

/**
 * @brief 1回の目的関数評価の結果
 *
 * 最適化器にはスカラ目的値と残差ベクトル全体を渡す。
 */
struct EvalResult {
    double objective = 0.0;           /**< 目的関数値（スカラ） */
    std::vector<double> residuals;   /**< 残差ベクトル（全製品連結） */
};

}  // namespace optimizer

#endif  // OPTIMIZER_CORE_EVAL_RESULT_H
