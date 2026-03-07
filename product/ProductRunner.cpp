#include "product/ProductRunner.h"

namespace optimizer {

ProductRunner::ProductRunner(IPhysicalModel& model, IProductDataLoader& loader)
    : model_(&model), loader_(&loader) {}

ProductResidualBlock ProductRunner::run(const ProductMeta& meta,
                                        const std::vector<double>& fullParams) {
    ProductResidualBlock block;
    block.product_id = meta.product_id;

    // USERWORK: 実際の製品バイナリファイル読込処理をここに差し替える（loader_ の実装で行う）
    std::unique_ptr<ProductLoadedData> data = loader_->load(meta);
    if (!data || data->measured.empty()) {
        block.ok = false;
        block.error_message = "Failed to load product data";
        return block;
    }

    // USERWORK: 実物理モデルの計算関数に置き換える
    std::vector<double> predicted = model_->run(fullParams, data.get());

    if (predicted.size() != data->measured.size()) {
        block.ok = false;
        block.error_message = "Model output size mismatch";
        return block;
    }

    block.residuals.resize(data->measured.size());
    for (size_t i = 0; i < data->measured.size(); ++i)
        block.residuals[i] = data->measured[i] - predicted[i];
    block.size = block.residuals.size();
    block.ok = true;
    return block;
}

}  // namespace optimizer
