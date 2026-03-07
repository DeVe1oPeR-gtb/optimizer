#include "mock/ResultWriterStub.h"

namespace optimizer {

void ResultWriterStub::writeApplyOnly(const std::vector<double>& /* fullParams */,
                                       const std::vector<ProductRunResult>& /* results */) {}

void ResultWriterStub::writeAfterOptimization(const std::vector<double>& /* fullParams */,
                                              const std::vector<ProductRunResult>& /* results */) {}

}  // namespace optimizer
