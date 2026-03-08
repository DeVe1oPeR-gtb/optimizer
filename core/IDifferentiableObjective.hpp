#ifndef OPTIMIZER_CORE_I_DIFFERENTIABLE_OBJECTIVE_H
#define OPTIMIZER_CORE_I_DIFFERENTIABLE_OBJECTIVE_H

/**
 * @file IDifferentiableObjective.h
 * @brief ヤコビアン付き目的関数インターフェース（Levenberg–Marquardt 用）
 */

#include "IObjective.hpp"
#include "JacobianResult.hpp"
#include <vector>

namespace optimizer {

/**
 * @brief 残差とヤコビアンを返せる目的関数（LM 用）
 *
 * evaluateWithJacobian(x) で残差ベクトルと jacobian[残差添字][パラメータ添字] を返す。
 * 本ライブラリでは Objective が数値微分で実装している。
 */
class IDifferentiableObjective : public IObjective {
public:
    ~IDifferentiableObjective() override = default;

    /**
     * @brief x での評価とヤコビアン（例: 数値微分）
     * @param x 最適化対象パラメータベクトル
     * @return 残差とヤコビ行列
     */
    virtual JacobianResult evaluateWithJacobian(const std::vector<double>& x) = 0;
};

}  // namespace optimizer

#endif  // OPTIMIZER_CORE_I_DIFFERENTIABLE_OBJECTIVE_H
