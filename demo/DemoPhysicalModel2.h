#ifndef DEMO_DEMO_PHYSICAL_MODEL2_H
#define DEMO_DEMO_PHYSICAL_MODEL2_H

/* デモ用・後で削除 */

#include "model/IPhysicalModel.h"

namespace optimizer {

/** デモ用モデル2: 線形 y = p0 + p1*x */
class DemoPhysicalModel2 : public IPhysicalModel {
public:
    std::vector<double> run(const std::vector<double>& fullParams,
                            const void* productLoadedData) override;
};

}  // namespace optimizer

#endif
