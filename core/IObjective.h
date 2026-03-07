#ifndef OPTIMIZER_CORE_I_OBJECTIVE_H
#define OPTIMIZER_CORE_I_OBJECTIVE_H

#include "EvalResult.h"
#include <vector>

namespace optimizer {

/**
 * @brief 目的関数評価の共通インターフェース
 *
 * 最適化器からは常に残差ベクトルを受け取り、目的スカラはそこから導出する。
 */
class IObjective {
public:
    virtual ~IObjective() = default;

    /**
     * @brief 最適化ベクトル x での評価（enable_opt されたパラメータのみ）
     * @param x 最適化対象パラメータベクトル
     * @return 目的値と残差ベクトル（全製品連結）
     */
    virtual EvalResult evaluate(const std::vector<double>& x) = 0;
};

}  // namespace optimizer

#endif  // OPTIMIZER_CORE_I_OBJECTIVE_H
