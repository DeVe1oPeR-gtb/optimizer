#ifndef OPTIMIZER_MODEL_I_PHYSICAL_MODEL_H
#define OPTIMIZER_MODEL_I_PHYSICAL_MODEL_H

#include <vector>

namespace optimizer {

/// USERWORK: Input/output types may be product-specific. Here we use generic vectors.
/// One product: one run returns one predicted vector (same length as measured / positions).

/// Interface for 1-product physical model. Called once per product per evaluation.
class IPhysicalModel {
public:
    virtual ~IPhysicalModel() = default;

    /// Run model for one product. Input: full parameter set for that product; product-loaded data.
    /// Returns predicted values at the same positions as measured (one-to-one).
    /// USERWORK: Replace with real physical model; input/output layout is product-specific.
    virtual std::vector<double> run(const std::vector<double>& fullParams,
                                   const void* productLoadedData) = 0;
};

}  // namespace optimizer

#endif  // OPTIMIZER_MODEL_I_PHYSICAL_MODEL_H
