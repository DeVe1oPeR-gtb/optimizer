/**
 * @file ProductLogBuffer.cpp
 * @brief PLOG バッファの実装。
 */

#include "util/ProductLogBuffer.hpp"
#include <cmath>
#include <iomanip>
#include <sstream>

namespace optimizer {

static double rmseFromResult(const ProductRunResult& r) {
    if (r.residuals.empty()) return 0.0;
    double sum = 0.0;
    for (double e : r.residuals) sum += e * e;
    return std::sqrt(sum / static_cast<double>(r.residuals.size()));
}

std::string ProductLogBuffer::formatNumeric(double value) {
    if (std::isfinite(value) && value == std::floor(value) && value >= -1e15 && value <= 1e15) {
        return std::to_string(static_cast<long long>(value));
    }
    std::ostringstream os;
    os << std::setprecision(6);
    if (std::abs(value) >= 1e6 || (std::abs(value) < 0.0001 && value != 0))
        os << std::scientific;
    os << value;
    return os.str();
}

void ProductLogBuffer::write(const std::vector<ProductRunResult>& results, const std::string& column_name) {
    const std::string rmseHeader = column_name.empty() ? "rmse" : column_name;
    if (plogRows_.empty()) {
        plogHeaders_ = {"product_id", rmseHeader};
        for (const auto& r : results) {
            if (!r.ok) continue;
            double rmse = rmseFromResult(r);
            plogRows_.push_back({r.product_id, formatNumeric(rmse)});
        }
        return;
    }
    plogHeaders_.push_back(rmseHeader);
    for (auto& row : plogRows_) {
        if (row.empty()) { row.push_back(""); continue; }
        const std::string& pid = row[0];
        std::string val;
        for (const auto& r : results) {
            if (!r.ok) continue;
            if (r.product_id == pid) { val = formatNumeric(rmseFromResult(r)); break; }
        }
        row.push_back(val);
    }
}

void ProductLogBuffer::clear() {
    plogHeaders_.clear();
    plogRows_.clear();
}

}  // namespace optimizer
