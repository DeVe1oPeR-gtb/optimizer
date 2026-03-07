#ifndef DEMO_DEMO_PHYSICAL_MODEL_H
#define DEMO_DEMO_PHYSICAL_MODEL_H

/* デモ用・後で削除する物理モデル */

#include "model/IPhysicalModel.h"

namespace optimizer {

/**
 * @brief デモ用の簡易物理モデル（2次多項式 y = p0 + p1*x + p2*x^2）
 * 後で削除する前提。
 */
class DemoPhysicalModel : public IPhysicalModel {
public:
    std::vector<double> run(const std::vector<double>& fullParams,
                            const void* productLoadedData) override;
};

}  // namespace optimizer

#endif  // DEMO_DEMO_PHYSICAL_MODEL_H
