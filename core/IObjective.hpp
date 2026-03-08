#ifndef OPTIMIZER_CORE_I_OBJECTIVE_H
#define OPTIMIZER_CORE_I_OBJECTIVE_H

/**
 * @file IObjective.h
 * @brief 目的関数の共通インターフェース（PSO/DE/LM が呼ぶ評価 API）
 */

#include "EvalResult.hpp"
#include <vector>

namespace optimizer {

/**
 * @brief 目的関数評価の共通インターフェース
 *
 * 最適化器（PSO/DE/LM）はこの evaluate(x) のみを呼ぶ。戻り値の残差ベクトルは
 * 全製品の残差を連結したもので、目的スカラ（例: RMSE）はそのノルムから導出する。
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
