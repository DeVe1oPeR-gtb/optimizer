#include "MockPhysicalModel.h"
#include "product/ProductLoadedData.h"
#include <cstddef>
#include <numeric>

namespace optimizer {

std::vector<double> MockPhysicalModel::run(const std::vector<double>& fullParams,
                                           const void* productLoadedData) {
    /* USERWORK: 実物理モデルの計算に置き換える */
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

}  // namespace optimizer
