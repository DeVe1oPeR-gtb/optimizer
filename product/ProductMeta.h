#ifndef OPTIMIZER_PRODUCT_PRODUCT_META_H
#define OPTIMIZER_PRODUCT_PRODUCT_META_H

#include <string>

namespace optimizer {

/// Lightweight metadata for one product (id, path, etc.).
struct ProductMeta {
    std::string product_id;
    std::string file_path;
};

}  // namespace optimizer

#endif  // OPTIMIZER_PRODUCT_PRODUCT_META_H
