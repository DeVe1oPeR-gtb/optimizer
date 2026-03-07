#ifndef OPTIMIZER_CORE_I_OBJECTIVE_H
#define OPTIMIZER_CORE_I_OBJECTIVE_H

#include "EvalResult.h"
#include <vector>

namespace optimizer {

/// Common interface for objective evaluation.
/// Optimizer receives always a residual vector; objective scalar is derived from it.
class IObjective {
public:
    virtual ~IObjective() = default;

    /// Evaluate at optimization vector x (only the enabled parameters).
    /// Returns objective value and full residual vector (all products concatenated).
    virtual EvalResult evaluate(const std::vector<double>& x) = 0;
};

}  // namespace optimizer

#endif  // OPTIMIZER_CORE_I_OBJECTIVE_H
