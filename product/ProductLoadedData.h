#ifndef OPTIMIZER_PRODUCT_PRODUCT_LOADED_DATA_H
#define OPTIMIZER_PRODUCT_PRODUCT_LOADED_DATA_H

#include <vector>

namespace optimizer {

/**
 * @brief 1製品分のロード済みデータ（実測値・位置等）
 *
 * USERWORK: レイアウトや項目は製品フォーマットに依存する。
 */
struct ProductLoadedData {
    std::vector<double> measured;   /**< 実測値（残差は measured - predicted） */
    std::vector<double> positions; /**< 位置等（measured と同長で 1:1 対応） */
};

}  // namespace optimizer

#endif  // OPTIMIZER_PRODUCT_PRODUCT_LOADED_DATA_H
