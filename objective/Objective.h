#ifndef OPTIMIZER_OBJECTIVE_OBJECTIVE_H
#define OPTIMIZER_OBJECTIVE_OBJECTIVE_H

#include "core/IDifferentiableObjective.h"
#include "param/ParameterMapper.h"
#include "product/BatchEvaluationHandler.h"
#include <memory>

namespace optimizer {

/**
 * @brief ParameterMapper と BatchEvaluationHandler を用いる目的関数
 *
 * evaluate(x): x を全パラメータに展開 → バッチ評価 → 目的値と残差を返す。
 * evaluateWithJacobian(x): 数値微分でヤコビアンを計算。
 */
class Objective : public IDifferentiableObjective {
public:
    /**
     * @param mapper 最適化ベクトル ⇔ 全パラメータの対応
     * @param batch 全製品の一括評価
     */
    Objective(ParameterMapper& mapper, BatchEvaluationHandler& batch);

    EvalResult evaluate(const std::vector<double>& x) override;
    JacobianResult evaluateWithJacobian(const std::vector<double>& x) override;

    /**
     * @brief ヤコビアン用の数値微分ステップ（デフォルト 1e-7 * (1 + |x_j|)）
     */
    void setEpsilon(double eps) { epsilon_ = eps; }
    double epsilon() const { return epsilon_; }

private:
    ParameterMapper* mapper_;
    BatchEvaluationHandler* batch_;
    double epsilon_ = 1e-7;
};

}  // namespace optimizer

#endif  // OPTIMIZER_OBJECTIVE_OBJECTIVE_H
