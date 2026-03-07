/* デモ用・後で削除 */

#include "demo/DemoPhysicalModel2.h"
#include "product/ProductLoadedData.h"
#include <cstddef>
#include <vector>

namespace optimizer {

std::vector<double> DemoPhysicalModel2::run(const std::vector<double>& fullParams,
                                            const void* productLoadedData) {
    const auto* data = static_cast<const ProductLoadedData*>(productLoadedData);
    if (!data || data->positions.empty()) return {};
    const double p0 = fullParams.size() > 0 ? fullParams[0] : 0.0;
    const double p1 = fullParams.size() > 1 ? fullParams[1] : 0.0;
    std::vector<double> pred(data->positions.size());
    for (std::size_t i = 0; i < data->positions.size(); ++i)
        pred[i] = p0 + p1 * data->positions[i];
    return pred;
}

}  // namespace optimizer
