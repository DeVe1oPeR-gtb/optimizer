#ifndef OPTIMIZER_MOCK_MOCK_H
#define OPTIMIZER_MOCK_MOCK_H

/* テスト用モック（本番では IPhysicalModel / IProductDataLoader の実装に差し替え） */

#include "model/IPhysicalModel.hpp"
#include "model/IProductDataLoader.hpp"

namespace optimizer {

/** テスト用モック物理モデル（パラメータの線形結合などダミー予測を返す） */
class MockPhysicalModel : public IPhysicalModel {
public:
    std::vector<double> run(const std::vector<double>& fullParams,
                            const void* productLoadedData) override;
};

/** テスト用モックデータローダ（ダミーの実測・位置データを返す） */
class MockProductDataLoader : public IProductDataLoader {
public:
    std::unique_ptr<ProductLoadedData> load(const ProductMeta& meta) override;
};

}  // namespace optimizer

#endif
