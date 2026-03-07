#include "MockProductDataLoader.h"
#include <memory>

namespace optimizer {

std::unique_ptr<ProductLoadedData> MockProductDataLoader::load(const ProductMeta& meta) {
    // USERWORK: Replace with actual product file load (binary, CSV, etc.).
    auto data = std::make_unique<ProductLoadedData>();
    size_t n = 4 + (meta.product_id.size() % 4);
    data->measured.resize(n, 1.0);
    data->positions.resize(n, 0.0);
    for (size_t i = 0; i < n; ++i) {
        data->measured[i] = 1.0 + i * 0.2;
        data->positions[i] = static_cast<double>(i);
    }
    return data;
}

}  // namespace optimizer
