#include "mock/Mock.hpp"
#include "product/ProductLoadedData.hpp"
#include <cstddef>
#include <memory>
#include <numeric>

namespace optimizer {

std::vector<double> MockPhysicalModel::run(const std::vector<double>& fullParams,
                                           const void* productLoadedData) {
    std::size_t n = 4;
    if (productLoadedData) {
        const auto* data = static_cast<const ProductLoadedData*>(productLoadedData);
        n = data->measured.size();
    }
    std::vector<double> pred(n, 0.0);
    double s = 0.0;
    for (std::size_t i = 0; i < fullParams.size() && i < 8; ++i)
        s += fullParams[i];
    for (std::size_t i = 0; i < n; ++i)
        pred[i] = s + static_cast<double>(i) * 0.1;
    return pred;
}

std::unique_ptr<ProductLoadedData> MockProductDataLoader::load(const ProductMeta& meta) {
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
