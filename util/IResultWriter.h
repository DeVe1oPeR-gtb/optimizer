#ifndef OPTIMIZER_UTIL_I_RESULT_WRITER_H
#define OPTIMIZER_UTIL_I_RESULT_WRITER_H

#include "product/ProductRunResult.h"
#include <vector>

namespace optimizer {

/**
 * @brief 計算結果の出力窓口
 *
 * USERWORK: オンサイトで本インターフェースを実装し、ファイル・DB 等へ出力する。
 * - 適用値のみ計算時: writeApplyOnly が呼ばれる（最適化は行わない）
 * - 最適化終了後: writeAfterOptimization が呼ばれる（最適化済みパラメータで各製品を再計算した結果）
 */
class IResultWriter {
public:
    virtual ~IResultWriter() = default;

    /** USERWORK: 現場仕様で実装。DB・設定の適用値で各製品を計算した結果を出力する。 */
    virtual void writeApplyOnly(const std::vector<double>& fullParams,
                                const std::vector<ProductRunResult>& results) = 0;

    /** USERWORK: 現場仕様で実装。最適化終了後、最適化済みパラメータで各製品を再計算した結果を出力する。 */
    virtual void writeAfterOptimization(const std::vector<double>& fullParams,
                                        const std::vector<ProductRunResult>& results) = 0;
};

}  // namespace optimizer

#endif
