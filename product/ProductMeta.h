#ifndef OPTIMIZER_PRODUCT_PRODUCT_META_H
#define OPTIMIZER_PRODUCT_PRODUCT_META_H

#include <string>

namespace optimizer {

/**
 * @brief 1製品分の軽量メタ情報（ID・ファイルパス等）
 */
struct ProductMeta {
    std::string product_id;   /**< 製品 ID */
    std::string file_path;   /**< データファイルパス等 */
};

}  // namespace optimizer

#endif  // OPTIMIZER_PRODUCT_PRODUCT_META_H
