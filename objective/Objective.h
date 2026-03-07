#ifndef OPTIMIZER_OBJECTIVE_OBJECTIVE_H
#define OPTIMIZER_OBJECTIVE_OBJECTIVE_H

#include "core/IDifferentiableObjective.h"
#include "param/ParameterMapper.h"
#include "product/BatchEvaluationHandler.h"
#include <memory>

namespace optimizer {

/// Objective that uses ParameterMapper + BatchEvaluationHandler.
/// evaluate(x) expands x to full params, runs batch evaluation, returns objective + residuals.
/// evaluateWithJacobian(x) uses finite differences.
class Objective : public IDifferentiableObjective {
public:
    Objective(ParameterMapper& mapper, BatchEvaluationHandler& batch);

    EvalResult evaluate(const std::vector<double>& x) override;
    JacobianResult evaluateWithJacobian(const std::vector<double>& x) override;

    /// Finite-difference step for Jacobian (default 1e-7 * (1 + |x_j|)).
    void setEpsilon(double eps) { epsilon_ = eps; }
    double epsilon() const { return epsilon_; }

private:
    ParameterMapper* mapper_;
    BatchEvaluationHandler* batch_;
    double epsilon_ = 1e-7;
};

}  // namespace optimizer

#endif  // OPTIMIZER_OBJECTIVE_OBJECTIVE_H
