#include "MockPhysicalModel.h"
#include <numeric>

namespace optimizer {

std::vector<double> MockPhysicalModel::run(const std::vector<double>& fullParams,
                                           const void* /* productLoadedData */) {
    // USERWORK: Replace with real physical model call.
    size_t n = 4 + (fullParams.size() % 4);
    std::vector<double> pred(n, 0.0);
    double s = 0.0;
    for (size_t i = 0; i < fullParams.size() && i < 8; ++i)
        s += fullParams[i];
    for (size_t i = 0; i < n; ++i)
        pred[i] = s + static_cast<double>(i) * 0.1;
    return pred;
}

}  // namespace optimizer
