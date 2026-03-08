#ifndef OPTIMIZER_PRODUCT_PRODUCT_META_H
#define OPTIMIZER_PRODUCT_PRODUCT_META_H

#include <string>
#include <vector>

namespace optimizer {

/**
 * @brief 1製品分の軽量メタ情報（ID・ファイルパス等）
 *
 * 現場でコイルNO・日付等が必要な場合は file_path にまとめるか、派生構造体で拡張する。
 */
struct ProductMeta {
    std::string product_id;   /**< 製品 ID */
    std::string file_path;   /**< データファイルパス等 */

    /**
     * @brief 最適化対象から外すデータ点の番号（0 始まりインデックス）
     *
     * USERWORK: 非定常部などの対象外判定ロジックはオンサイトで組み、製品リスト構築時に
     * ここに対象外のインデックスをセットする。空のときは全データ点を対象とする。
     */
    std::vector<size_t> excluded_data_indices;
};

}  // namespace optimizer

#endif  // OPTIMIZER_PRODUCT_PRODUCT_META_H
