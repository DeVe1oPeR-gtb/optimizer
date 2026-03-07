#include "objective/Objective.h"
#include "product/ResidualAssembly.h"
#include <cmath>
#include <numeric>

namespace optimizer {

Objective::Objective(ParameterMapper& mapper, BatchEvaluationHandler& batch)
    : mapper_(&mapper), batch_(&batch) {}

EvalResult Objective::evaluate(const std::vector<double>& x) {
    std::vector<double> full = mapper_->expandToFullParameterSet(x);
    ResidualAssembly asm_ = batch_->evaluate(full);
    EvalResult out;
    out.residuals = std::move(asm_.full_residuals);
    // USERWORK: Replace with real objective formula (e.g. weighted sum of squares).
    double ss = 0.0;
    for (double r : out.residuals)
        ss += r * r;
    out.objective = std::sqrt(ss);  // sqrt(sum of squares) as scalar
    return out;
}

JacobianResult Objective::evaluateWithJacobian(const std::vector<double>& x) {
    EvalResult base = evaluate(x);
    JacobianResult out;
    out.residuals = base.residuals;
    const size_t nRes = out.residuals.size();
    const size_t nPar = x.size();
    out.jacobian.resize(nRes);
    for (size_t i = 0; i < nRes; ++i)
        out.jacobian[i].resize(nPar, 0.0);

    double eps = epsilon_;
    for (size_t j = 0; j < nPar; ++j) {
        double h = eps * (1.0 + std::fabs(x[j]));
        if (h < eps) h = eps;
        std::vector<double> x_plus = x;
        x_plus[j] += h;
        EvalResult er_plus = evaluate(x_plus);
        if (er_plus.residuals.size() != nRes) continue;
        for (size_t i = 0; i < nRes; ++i)
            out.jacobian[i][j] = (er_plus.residuals[i] - base.residuals[i]) / h;
    }
    return out;
}

}  // namespace optimizer
