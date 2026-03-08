/**
 * @file ProductLogBuffer.cpp
 * @brief PLOG バッファの実装。
 */

#include "util/ProductLogBuffer.hpp"
#include <algorithm>
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

void ProductLogBuffer::ensureHeader(const std::string& header) {
    if (plogHeaders_.empty() || std::find(plogHeaders_.begin(), plogHeaders_.end(), header) == plogHeaders_.end())
        plogHeaders_.push_back(header);
}

void ProductLogBuffer::plogAdd(const std::string& header, const std::string& value) {
    ensureHeader(header);
    size_t i = static_cast<size_t>(std::find(plogHeaders_.begin(), plogHeaders_.end(), header) - plogHeaders_.begin());
    if (plogCurrentRow_.size() <= i) plogCurrentRow_.resize(i + 1, "");
    plogCurrentRow_[i] = value;
}

void ProductLogBuffer::plogEndRow() {
    if (!plogCurrentRow_.empty()) {
        plogRows_.push_back(std::move(plogCurrentRow_));
        plogCurrentRow_.clear();
    }
}

size_t ProductLogBuffer::estimateBytes() const {
    size_t n = 0;
    for (const auto& h : plogHeaders_) n += h.size() + 2;
    for (const auto& row : plogRows_)
        for (const auto& cell : row) n += cell.size() + 2;
    n += 200;
    return n;
}

void ProductLogBuffer::clear() {
    plogHeaders_.clear();
    plogRows_.clear();
    plogCurrentRow_.clear();
}

}  // namespace optimizer
