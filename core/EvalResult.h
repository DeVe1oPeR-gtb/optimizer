#ifndef OPTIMIZER_CORE_EVAL_RESULT_H
#define OPTIMIZER_CORE_EVAL_RESULT_H

#include <vector>

namespace optimizer {

/// Result of a single objective evaluation.
/// Optimizer sees a scalar objective and the full residual vector.
struct EvalResult {
    double objective = 0.0;
    std::vector<double> residuals;
};

}  // namespace optimizer

#endif  // OPTIMIZER_CORE_EVAL_RESULT_H
