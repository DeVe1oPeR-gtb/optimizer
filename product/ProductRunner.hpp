#ifndef OPTIMIZER_PRODUCT_PRODUCT_RUNNER_H
#define OPTIMIZER_PRODUCT_PRODUCT_RUNNER_H

#include "product/ProductMeta.hpp"
#include "product/ProductLoadedData.hpp"
#include "product/ProductResidualBlock.hpp"
#include "model/IPhysicalModel.hpp"
#include "model/IProductDataLoader.hpp"
#include <memory>
#include <vector>

namespace optimizer {

/**
 * @brief 1製品の評価: データ読込 → 物理モデル実行 → 残差ブロック作成（実測 - 予測）
 */
class ProductRunner {
public:
    /**
     * @param model 1製品用物理モデル
     * @param loader 製品データローダ
     */
    ProductRunner(IPhysicalModel& model, IProductDataLoader& loader);

    /**
     * @brief 1製品を評価する
     * @param meta 製品メタ情報
     * @param fullParams 全パラメータ（ParameterMapper の並びと同じ）
     * @return 残差ブロック
     */
    ProductResidualBlock run(const ProductMeta& meta, const std::vector<double>& fullParams);

private:
    IPhysicalModel* model_;
    IProductDataLoader* loader_;
};

}  // namespace optimizer

#endif  // OPTIMIZER_PRODUCT_PRODUCT_RUNNER_H
