#ifndef OPTIMIZER_PRODUCT_PRODUCT_RUN_RESULT_H
#define OPTIMIZER_PRODUCT_PRODUCT_RUN_RESULT_H

#include <string>
#include <utility>
#include <vector>

namespace optimizer {

/**
 * @brief 1製品分の計算結果（実測・予測・残差）
 *
 * IResultWriter::writeApplyOnly / writeAfterOptimization の引数として渡される。
 * USERWORK: オンサイトの IResultWriter 実装で、この内容を現場仕様の形式で出力する。
 * extra_columns は ProductLoadedData からコピーされ、PLOG/LLOG/DLOG 等の結果 CSV の追加列として使える。
 * LLOG/DLOG では統計量は出さず、この extra_columns で製品情報をユーザー側が自由に追加する用途。
 */
struct ProductRunResult {
    std::string product_id;
    std::vector<double> measured;   /**< 実測値 */
    std::vector<double> predicted; /**< 予測値（measured と同長） */
    std::vector<double> residuals; /**< 残差（measured - predicted） */
    bool ok = true;
    std::string error_message;
    /** 追加列（名前, 値）。ローダで ProductLoadedData::extra_columns に設定すると LLOG/DLOG にそのまま出力される。製品情報を自由に追加する用。 */
    std::vector<std::pair<std::string, std::string>> extra_columns;
};

}  // namespace optimizer

#endif
