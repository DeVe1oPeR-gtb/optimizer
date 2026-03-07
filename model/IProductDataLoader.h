#ifndef OPTIMIZER_MODEL_I_PRODUCT_DATA_LOADER_H
#define OPTIMIZER_MODEL_I_PRODUCT_DATA_LOADER_H

#include "product/ProductLoadedData.h"
#include "product/ProductMeta.h"
#include <memory>

namespace optimizer {

/**
 * @brief 1製品分のデータをロードするインターフェース
 *
 * USERWORK: 実ファイル/DB からの読込実装に差し替える。
 */
class IProductDataLoader {
public:
    virtual ~IProductDataLoader() = default;

    /**
     * @brief 指定製品のデータをロードする
     * @param meta 製品メタ（ID・パス等）
     * @return ロード済みデータ。失敗時は nullptr
     */
    virtual std::unique_ptr<ProductLoadedData> load(const ProductMeta& meta) = 0;
};

}  // namespace optimizer

#endif  // OPTIMIZER_MODEL_I_PRODUCT_DATA_LOADER_H
