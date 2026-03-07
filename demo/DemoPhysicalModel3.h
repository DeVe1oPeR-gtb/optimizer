#ifndef DEMO_DEMO_PHYSICAL_MODEL3_H
#define DEMO_DEMO_PHYSICAL_MODEL3_H

/* デモ用・後で削除 */

#include "model/IPhysicalModel.h"

namespace optimizer {

/** デモ用モデル3: y = p0 + p1/(1+x) + p2*exp(-x/5) */
class DemoPhysicalModel3 : public IPhysicalModel {
public:
    std::vector<double> run(const std::vector<double>& fullParams,
                            const void* productLoadedData) override;
};

}  // namespace optimizer

#endif
