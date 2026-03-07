/* デモ用・後で削除。モデル2用: 真値 y = 1.0 - 0.3*x */

#include "demo/DemoDataLoader2.h"
#include <memory>

namespace optimizer {

std::unique_ptr<ProductLoadedData> DemoDataLoader2::load(const ProductMeta& /* meta */) {
    auto data = std::make_unique<ProductLoadedData>();
    const size_t n = 10;
    data->positions.resize(n);
    data->measured.resize(n);
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        data->positions[i] = x;
        data->measured[i] = 1.0 - 0.3 * x;
    }
    return data;
}

}  // namespace optimizer
