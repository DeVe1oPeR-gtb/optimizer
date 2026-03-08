#ifndef OPTIMIZER_PRODUCT_BATCH_EVALUATION_HANDLER_H
#define OPTIMIZER_PRODUCT_BATCH_EVALUATION_HANDLER_H

#include "product/ProductMeta.hpp"
#include "product/ResidualAssembly.hpp"
#include "product/ProductRunner.hpp"
#include <vector>

namespace optimizer {

/**
 * @brief アルゴリズム非依存のバッチ評価
 *
 * 製品リストに対して ProductRunner を順に呼び、残差ブロックを連結する。
 */
class BatchEvaluationHandler {
public:
    explicit BatchEvaluationHandler(ProductRunner& runner);

    /**
     * @brief 評価対象製品リストを設定（メタのみ。データは実行時に読込）
     */
    void setProducts(std::vector<ProductMeta> products);

    /**
     * @brief 全製品を指定の全パラメータで評価し、連結残差とブロックを返す
     * @param fullParams 全パラメータベクトル
     * @return 連結残差と製品ごとのブロック
     */
    ResidualAssembly evaluate(const std::vector<double>& fullParams);

private:
    ProductRunner* runner_;
    std::vector<ProductMeta> products_;
};

}  // namespace optimizer

#endif  // OPTIMIZER_PRODUCT_BATCH_EVALUATION_HANDLER_H
