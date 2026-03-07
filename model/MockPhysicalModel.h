#ifndef OPTIMIZER_MODEL_MOCK_PHYSICAL_MODEL_H
#define OPTIMIZER_MODEL_MOCK_PHYSICAL_MODEL_H

#include "IPhysicalModel.h"

namespace optimizer {

/// Mock: returns dummy predictions (e.g. sum of first few params) for testing.
class MockPhysicalModel : public IPhysicalModel {
public:
    std::vector<double> run(const std::vector<double>& fullParams,
                           const void* productLoadedData) override;
};

}  // namespace optimizer

#endif  // OPTIMIZER_MODEL_MOCK_PHYSICAL_MODEL_H
