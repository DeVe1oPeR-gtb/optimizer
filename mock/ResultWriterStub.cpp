/**
 * @file ResultWriterStub.cpp
 * @brief IResultWriter のスタブ（何も出力しない）。USERWORK: 現場では IResultWriter を実装したクラスに差し替える。
 */

#include "mock/ResultWriterStub.hpp"

namespace optimizer {

// 結果出力が不要なテスト・デモ用。何も書き出さないので IResultWriter のインターフェースだけ満たす。
void ResultWriterStub::writeApplyOnly(const std::vector<double>& /* fullParams */,
                                       const std::vector<ProductRunResult>& /* results */) {}

void ResultWriterStub::writeAfterOptimization(const std::vector<double>& /* fullParams */,
                                              const std::vector<ProductRunResult>& /* results */) {}

}  // namespace optimizer
