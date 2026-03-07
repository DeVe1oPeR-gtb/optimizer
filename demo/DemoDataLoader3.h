#ifndef DEMO_DEMO_DATA_LOADER3_H
#define DEMO_DEMO_DATA_LOADER3_H

#include "model/IProductDataLoader.h"

namespace optimizer {

/** デモ用データローダ3: 真値 y = 0.5 + 2.0/(1+x) + 0.1*exp(-x/5) */
class DemoDataLoader3 : public IProductDataLoader {
public:
    std::unique_ptr<ProductLoadedData> load(const ProductMeta& meta) override;
};

}  // namespace optimizer

#endif
