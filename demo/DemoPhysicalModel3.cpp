/* デモ用・後で削除 */

#include "demo/DemoPhysicalModel3.h"
#include "product/ProductLoadedData.h"
#include <cmath>
#include <cstddef>
#include <vector>

namespace optimizer {

std::vector<double> DemoPhysicalModel3::run(const std::vector<double>& fullParams,
                                            const void* productLoadedData) {
    const auto* data = static_cast<const ProductLoadedData*>(productLoadedData);
    if (!data || data->positions.empty()) return {};
    const double p0 = fullParams.size() > 0 ? fullParams[0] : 0.0;
    const double p1 = fullParams.size() > 1 ? fullParams[1] : 0.0;
    const double p2 = fullParams.size() > 2 ? fullParams[2] : 0.0;
    std::vector<double> pred(data->positions.size());
    for (std::size_t i = 0; i < data->positions.size(); ++i) {
        double x = data->positions[i];
        pred[i] = p0 + p1 / (1.0 + x) + p2 * std::exp(-x / 5.0);
    }
    return pred;
}

}  // namespace optimizer
