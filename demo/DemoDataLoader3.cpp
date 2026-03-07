/* デモ用・後で削除 */

#include "demo/DemoDataLoader3.h"
#include <cmath>
#include <memory>

namespace optimizer {

std::unique_ptr<ProductLoadedData> DemoDataLoader3::load(const ProductMeta& /* meta */) {
    auto data = std::make_unique<ProductLoadedData>();
    const size_t n = 10;
    data->positions.resize(n);
    data->measured.resize(n);
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        data->positions[i] = x;
        data->measured[i] = 0.5 + 2.0 / (1.0 + x) + 0.1 * std::exp(-x / 5.0);
    }
    return data;
}

}  // namespace optimizer
