/* デモ用・後で削除 */

#include "demo/DemoDataLoader.h"
#include <memory>
#include <cmath>

namespace optimizer {

std::unique_ptr<ProductLoadedData> DemoDataLoader::load(const ProductMeta& /* meta */) {
    auto data = std::make_unique<ProductLoadedData>();
    const size_t n = 10;
    data->positions.resize(n);
    data->measured.resize(n);
    /* 真のモデル: y = 1.0 - 0.5*x + 0.1*x^2。最適化でこの係数に近づくことを確認する。 */
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        data->positions[i] = x;
        data->measured[i] = 1.0 - 0.5 * x + 0.1 * x * x;
    }
    return data;
}

}  // namespace optimizer
