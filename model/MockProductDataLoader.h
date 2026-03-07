#ifndef OPTIMIZER_MODEL_MOCK_PRODUCT_DATA_LOADER_H
#define OPTIMIZER_MODEL_MOCK_PRODUCT_DATA_LOADER_H

#include "IProductDataLoader.h"

namespace optimizer {

/**
 * @brief テスト用モックデータローダ
 *
 * ダミーの実測・位置データを返す。
 */
class MockProductDataLoader : public IProductDataLoader {
public:
    std::unique_ptr<ProductLoadedData> load(const ProductMeta& meta) override;
};

}  // namespace optimizer

#endif  // OPTIMIZER_MODEL_MOCK_PRODUCT_DATA_LOADER_H
