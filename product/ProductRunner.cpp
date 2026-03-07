/**
 * @file ProductRunner.cpp
 * @brief 1 製品の評価: データ読込 → モデル実行 → 残差ブロック作成（対象外インデックスは除く）
 */

#include "product/ProductRunner.h"

namespace optimizer {

ProductRunner::ProductRunner(IPhysicalModel& model, IProductDataLoader& loader)
    : model_(&model), loader_(&loader) {}

ProductResidualBlock ProductRunner::run(const ProductMeta& meta,
                                        const std::vector<double>& fullParams) {
    ProductResidualBlock block;
    block.product_id = meta.product_id;

    /* USERWORK: 実際の製品バイナリファイル読込処理は loader_ の実装で差し替える。コイルごとに再ロードすること。 */
    std::unique_ptr<ProductLoadedData> data = loader_->load(meta);
    if (!data || data->measured.empty()) {
        block.ok = false;
        block.error_message = "Failed to load product data";
        return block;
    }

    /* USERWORK: 実物理モデルの計算関数に置き換える */
    std::vector<double> predicted = model_->run(fullParams, data.get());

    if (predicted.size() != data->measured.size()) {
        block.ok = false;
        block.error_message = "Model output size mismatch";
        return block;
    }

    /* 対象外データ点（excluded_data_indices）は残差に含めず、最適化の目的関数に寄与しない。 */
    const size_t n = data->measured.size();
    auto isExcluded = [&meta](size_t i) {
        for (size_t e : meta.excluded_data_indices)
            if (e == i) return true;
        return false;
    };
    block.residuals.clear();
    block.residuals.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        if (isExcluded(i)) continue;
        block.residuals.push_back(data->measured[i] - predicted[i]);
    }
    block.size = block.residuals.size();
    block.ok = true;
    return block;
}

}  // namespace optimizer
