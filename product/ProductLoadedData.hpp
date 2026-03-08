#ifndef OPTIMIZER_PRODUCT_PRODUCT_LOADED_DATA_H
#define OPTIMIZER_PRODUCT_PRODUCT_LOADED_DATA_H

/**
 * @file ProductLoadedData.h
 * @brief 1 製品分のロード済みデータ（実測・位置。残差は measured - predicted）
 */

#include <string>
#include <utility>
#include <vector>

namespace optimizer {

/**
 * @brief 実績・予測の 1 種類分（製品ごとに複数種類あり得る）
 *
 * cfg の optimization_data_types=A,B で最適化に使用する種類を選択する。
 * TraceConfig::isDataTypeUsedForOptimization(data_type_id) で使用/不使用を判定。
 */
struct ProductDataSet {
    std::string data_type_id;       /**< 種類 ID（例: "thickness", "profile"。空は従来互換の 1 種類） */
    std::vector<double> measured;   /**< 実測値 */
    std::vector<double> positions;  /**< 位置等（measured と同長で 1:1 対応） */
};

/**
 * @brief 1製品分のロード済みデータ（実測値・位置等）
 *
 * USERWORK: レイアウトや項目は製品フォーマットに依存する。
 * extra_columns にコイル番号・厚み等を入れておくと、結果 CSV にそのまま出力される。
 * 製品ごとに複数種類の実績・予測がある場合は data_sets に追加する。data_sets が空のときは
 * measured/positions を 1 種類として扱う（data_type_id は空とみなす）。
 */
struct ProductLoadedData {
    std::vector<double> measured;   /**< 実測値（data_sets 未使用時の 1 種類。残差は measured - predicted） */
    std::vector<double> positions; /**< 位置等（measured と同長で 1:1 対応） */
    /** 複数種類の実績・予測セット。非空のときはこちらを優先し、最適化では isDataTypeUsedForOptimization が true の種類のみ使用。 */
    std::vector<ProductDataSet> data_sets;
    /** CSV 用の追加列（名前, 値）。ローダで設定すると ProductRunResult 経由で結果 CSV に出力可能。値は文字列で保持。 */
    std::vector<std::pair<std::string, std::string>> extra_columns;
};

}  // namespace optimizer

#endif  // OPTIMIZER_PRODUCT_PRODUCT_LOADED_DATA_H
