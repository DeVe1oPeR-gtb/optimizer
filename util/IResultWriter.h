#ifndef OPTIMIZER_UTIL_I_RESULT_WRITER_H
#define OPTIMIZER_UTIL_I_RESULT_WRITER_H

#include "product/ProductRunResult.h"
#include <vector>

namespace optimizer {

/**
 * @brief 計算結果の出力窓口（オンサイトで実装する）
 *
 * - 適用値のみ計算時: writeApplyOnly が呼ばれる（最適化は行わない）
 * - 最適化終了後: writeAfterOptimization が呼ばれる（最適化済みパラメータで各製品を再計算した結果）
 */
class IResultWriter {
public:
    virtual ~IResultWriter() = default;

    /**
     * @brief DB・設定の適用値で各製品を計算した結果を出力
     * @param fullParams 適用した全パラメータ（DB + 設定ファイルの初期値から取得）
     * @param results 製品ごとの実測・予測・残差
     */
    virtual void writeApplyOnly(const std::vector<double>& fullParams,
                                const std::vector<ProductRunResult>& results) = 0;

    /**
     * @brief 最適化終了後、最適化済みパラメータで各製品を計算した結果を出力
     * @param fullParams 最適化で得られた全パラメータ（最適化ベクトルを展開したもの）
     * @param results 製品ごとの実測・予測・残差
     */
    virtual void writeAfterOptimization(const std::vector<double>& fullParams,
                                        const std::vector<ProductRunResult>& results) = 0;
};

}  // namespace optimizer

#endif
