#ifndef OPTIMIZER_MOCK_DEMO_H
#define OPTIMIZER_MOCK_DEMO_H

/* デモ用モデル・データローダ（後で削除する前提） */

#include "model/IPhysicalModel.h"
#include "model/IProductDataLoader.h"

namespace optimizer {

/** デモ用: 2次多項式 y = p0 + p1*x + p2*x^2 */
class DemoPhysicalModel : public IPhysicalModel {
public:
    std::vector<double> run(const std::vector<double>& fullParams,
                            const void* productLoadedData) override;
};
/** デモ用: 線形 y = p0 + p1*x */
class DemoPhysicalModel2 : public IPhysicalModel {
public:
    std::vector<double> run(const std::vector<double>& fullParams,
                            const void* productLoadedData) override;
};
/** デモ用: y = p0 + p1/(1+x) + p2*exp(-x/5) */
class DemoPhysicalModel3 : public IPhysicalModel {
public:
    std::vector<double> run(const std::vector<double>& fullParams,
                            const void* productLoadedData) override;
};

/** デモ用: 真値 y = 1 - 0.5*x + 0.1*x^2 の点列 */
class DemoDataLoader : public IProductDataLoader {
public:
    std::unique_ptr<ProductLoadedData> load(const ProductMeta& meta) override;
};
/** デモ用: 真値 y = 1.0 - 0.3*x */
class DemoDataLoader2 : public IProductDataLoader {
public:
    std::unique_ptr<ProductLoadedData> load(const ProductMeta& meta) override;
};
/** デモ用: 真値 y = 0.5 + 2.0/(1+x) + 0.1*exp(-x/5) */
class DemoDataLoader3 : public IProductDataLoader {
public:
    std::unique_ptr<ProductLoadedData> load(const ProductMeta& meta) override;
};

}  // namespace optimizer

#endif
