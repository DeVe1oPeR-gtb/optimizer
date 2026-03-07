#ifndef OPTIMIZER_CORE_I_DIFFERENTIABLE_OBJECTIVE_H
#define OPTIMIZER_CORE_I_DIFFERENTIABLE_OBJECTIVE_H

#include "IObjective.h"
#include "JacobianResult.h"
#include <vector>

namespace optimizer {

/// Objective that can provide residuals and Jacobian (for LM).
class IDifferentiableObjective : public IObjective {
public:
    ~IDifferentiableObjective() override = default;

    /// Evaluate at x and return residuals plus Jacobian (e.g. by finite differences).
    virtual JacobianResult evaluateWithJacobian(const std::vector<double>& x) = 0;
};

}  // namespace optimizer

#endif  // OPTIMIZER_CORE_I_DIFFERENTIABLE_OBJECTIVE_H
