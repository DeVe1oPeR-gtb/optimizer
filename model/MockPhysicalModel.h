#ifndef OPTIMIZER_MODEL_MOCK_PHYSICAL_MODEL_H
#define OPTIMIZER_MODEL_MOCK_PHYSICAL_MODEL_H

#include "IPhysicalModel.h"

namespace optimizer {

/**
 * @brief テスト用モック物理モデル
 *
 * パラメータの線形結合などダミー予測を返す。
 */
class MockPhysicalModel : public IPhysicalModel {
public:
    std::vector<double> run(const std::vector<double>& fullParams,
                           const void* productLoadedData) override;
};

}  // namespace optimizer

#endif  // OPTIMIZER_MODEL_MOCK_PHYSICAL_MODEL_H
