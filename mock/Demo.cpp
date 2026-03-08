/**
 * @file Demo.cpp
 * @brief デモ用の 3 モデル（quadratic, linear, rational_exp）と 3 ローダ。measured を式で生成。USERWORK: 現場では IPhysicalModel / IProductDataLoader を実装に差し替える。
 */

#include "mock/Demo.hpp"
#include "product/ProductLoadedData.hpp"
#include <cmath>
#include <cstddef>
#include <memory>
#include <vector>

namespace optimizer {

// デモ用の 2 次多項式モデル。pred = p0 + p1*x + p2*x^2。現場では IPhysicalModel を実装に差し替える。
static std::vector<double> runQuadratic(const std::vector<double>& fullParams,
                                        const void* productLoadedData) {
    const auto* data = static_cast<const ProductLoadedData*>(productLoadedData);
    if (!data || data->positions.empty()) return {};
    const double p0 = fullParams.size() > 0 ? fullParams[0] : 0.0;
    const double p1 = fullParams.size() > 1 ? fullParams[1] : 0.0;
    const double p2 = fullParams.size() > 2 ? fullParams[2] : 0.0;
    std::vector<double> pred(data->positions.size());
    for (std::size_t i = 0; i < data->positions.size(); ++i) {
        double x = data->positions[i];
        pred[i] = p0 + p1 * x + p2 * x * x;
    }
    return pred;
}

std::vector<double> DemoPhysicalModel::run(const std::vector<double>& fullParams,
                                           const void* productLoadedData) {
    return runQuadratic(fullParams, productLoadedData);
}

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

std::unique_ptr<ProductLoadedData> DemoDataLoader::load(const ProductMeta&) {
    auto data = std::make_unique<ProductLoadedData>();
    const size_t n = 10;
    data->positions.resize(n);
    data->measured.resize(n);
    // デモ用に 1.0 - 0.5*x + 0.1*x^2 で measured を生成。runQuadratic と合わせると最適化で再現できる。
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        data->positions[i] = x;
        data->measured[i] = 1.0 - 0.5 * x + 0.1 * x * x;
    }
    return data;
}

std::unique_ptr<ProductLoadedData> DemoDataLoader2::load(const ProductMeta&) {
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

std::unique_ptr<ProductLoadedData> DemoDataLoader3::load(const ProductMeta&) {
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
