#ifndef OPTIMIZER_CORE_JACOBIAN_RESULT_H
#define OPTIMIZER_CORE_JACOBIAN_RESULT_H

#include <vector>

namespace optimizer {

/// Result of evaluation with Jacobian (for LM and other gradient-based methods).
/// jacobian[residual_index][param_index] = d(residual_i) / d(x_j).
struct JacobianResult {
    std::vector<double> residuals;
    /// Jacobian: [num_residuals][num_params], row-major
    std::vector<std::vector<double>> jacobian;
};

}  // namespace optimizer

#endif  // OPTIMIZER_CORE_JACOBIAN_RESULT_H
