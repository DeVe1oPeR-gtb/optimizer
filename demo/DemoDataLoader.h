#ifndef DEMO_DEMO_DATA_LOADER_H
#define DEMO_DEMO_DATA_LOADER_H

/* デモ用・後で削除 */

#include "model/IProductDataLoader.h"

namespace optimizer {

/**
 * @brief デモ用データローダ（真値 y = 1 - 0.5*x + 0.1*x^2 の点列を返す）
 * 後で削除する前提。
 */
class DemoDataLoader : public IProductDataLoader {
public:
    std::unique_ptr<ProductLoadedData> load(const ProductMeta& meta) override;
};

}  // namespace optimizer

#endif  // DEMO_DEMO_DATA_LOADER_H
