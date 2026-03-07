#ifndef OPTIMIZER_PRODUCT_PRODUCT_META_H
#define OPTIMIZER_PRODUCT_PRODUCT_META_H

#include <string>

namespace optimizer {

/**
 * @brief 1製品分の軽量メタ情報（ID・ファイルパス等）
 *
 * 現場でコイルNO・日付等が必要な場合は file_path にまとめるか、派生構造体で拡張する。
 */
struct ProductMeta {
    std::string product_id;   /**< 製品 ID */
    std::string file_path;   /**< データファイルパス等 */
};

}  // namespace optimizer

#endif  // OPTIMIZER_PRODUCT_PRODUCT_META_H
