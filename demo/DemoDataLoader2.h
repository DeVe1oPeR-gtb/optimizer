#ifndef DEMO_DEMO_DATA_LOADER2_H
#define DEMO_DEMO_DATA_LOADER2_H

#include "model/IProductDataLoader.h"

namespace optimizer {

/** デモ用データローダ2: 真値 y = 1.0 - 0.3*x */
class DemoDataLoader2 : public IProductDataLoader {
public:
    std::unique_ptr<ProductLoadedData> load(const ProductMeta& meta) override;
};

}  // namespace optimizer

#endif
