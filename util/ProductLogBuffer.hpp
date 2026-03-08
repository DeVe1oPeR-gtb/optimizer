/**
 * @file ProductLogBuffer.hpp
 * @brief PLOG 用バッファ。1 製品 1 行・列を横に追加。終了時に flush で 1 ファイルに書き出す。
 */

#ifndef OPTIMIZER_UTIL_PRODUCT_LOG_BUFFER_H
#define OPTIMIZER_UTIL_PRODUCT_LOG_BUFFER_H

#include "product/ProductRunResult.hpp"
#include <cstddef>
#include <string>
#include <vector>

namespace optimizer {

/**
 * @brief PLOG (product log) 専用バッファ
 *
 * - setFilename(format): 出力先フォーマット（{timestamp}, {product_id} 等）
 * - write(results, column_name): 列を横に追加（複数回で列が増える）
 * - getFilename / getHeaders / getRows: 内容取得（ResultOutput が flush 時にファイル書き出しに使用）
 * - clear(): 書き出し後にクリア
 */
class ProductLogBuffer {
public:
    void setFilename(const std::string& format) { plogFilename_ = format; }
    void write(const std::vector<ProductRunResult>& results, const std::string& column_name);
    const std::string& getFilename() const { return plogFilename_; }
    const std::vector<std::string>& getHeaders() const { return plogHeaders_; }
    const std::vector<std::vector<std::string>>& getRows() const { return plogRows_; }
    void clear();
    bool empty() const { return plogRows_.empty(); }

private:
    std::string plogFilename_;
    std::vector<std::string> plogHeaders_;
    std::vector<std::vector<std::string>> plogRows_;
    static std::string formatNumeric(double value);
};

}  // namespace optimizer

#endif
