/**
 * @file ResultOutput.cpp
 * @brief 結果 CSV 書き出しヘルパの実装。
 */

#include "util/ResultOutput.hpp"
#include "util/util_common.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace optimizer {

ResultOutput::~ResultOutput() {
    if (dlogFile_.is_open()) {
        dlogFile_.flush();
        dlogFile_.close();
    }
}

static std::string escapeCsvCell(const std::string& s) {
    if (s.find(',') == std::string::npos && s.find('"') == std::string::npos && s.find('\n') == std::string::npos)
        return s;
    std::ostringstream out;
    out << '"';
    for (char c : s) {
        if (c == '"') out << "\"\"";
        else out << c;
    }
    out << '"';
    return out.str();
}

void ResultOutput::ensureHeader(const std::string& header) {
    if (headers_.empty() || std::find(headers_.begin(), headers_.end(), header) == headers_.end())
        headers_.push_back(header);
}

void ResultOutput::addColumn(const std::string& header, const std::string& value) {
    ensureHeader(header);
    size_t i = static_cast<size_t>(std::find(headers_.begin(), headers_.end(), header) - headers_.begin());
    if (currentRow_.size() <= i) currentRow_.resize(i + 1, "");
    currentRow_[i] = value;
}

void ResultOutput::addColumn(const std::string& header, double value, const std::string& format) {
    ensureHeader(header);
    size_t i = static_cast<size_t>(std::find(headers_.begin(), headers_.end(), header) - headers_.begin());
    if (currentRow_.size() <= i) currentRow_.resize(i + 1, "");
    currentRow_[i] = formatNumeric(value, format);
}

void ResultOutput::addColumn(const std::string& header, int value, const std::string& format) {
    ensureHeader(header);
    size_t i = static_cast<size_t>(std::find(headers_.begin(), headers_.end(), header) - headers_.begin());
    if (currentRow_.size() <= i) currentRow_.resize(i + 1, "");
    currentRow_[i] = formatNumeric(value, format);
}

std::string ResultOutput::formatNumeric(double value, const std::string& format) const {
    if (!format.empty()) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), format.c_str(), value);
        return buf;
    }
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

std::string ResultOutput::formatNumeric(int value, const std::string& format) const {
    if (!format.empty()) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), format.c_str(), value);
        return buf;
    }
    return std::to_string(value);
}

void ResultOutput::endRow() {
    if (!currentRow_.empty()) {
        rows_.push_back(std::move(currentRow_));
        currentRow_.clear();
    }
}

void ResultOutput::setFilename(ResultTiming timing, const std::string& format) {
    if (timing == ResultTiming::BeforeOptimization)
        filenameBefore_ = format;
    else
        filenameAfter_ = format;
}

std::string ResultOutput::getFilename(ResultTiming timing) const {
    return timing == ResultTiming::BeforeOptimization ? filenameBefore_ : filenameAfter_;
}

std::string ResultOutput::replacePlaceholders(const std::string& fmt, const std::string& product_id) const {
    std::string out = fmt;
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream ts;
    ts << std::put_time(std::localtime(&t), "%Y%m%d_%H%M%S");
    std::string timestamp = ts.str();
    size_t pos = 0;
    while ((pos = out.find("{timestamp}", pos)) != std::string::npos) {
        out.replace(pos, 11, timestamp);
        pos += timestamp.size();
    }
    pos = 0;
    while ((pos = out.find("{product_id}", pos)) != std::string::npos) {
        out.replace(pos, 12, product_id);
        pos += product_id.size();
    }
    return out;
}

bool ResultOutput::wouldExceedLimits(size_t appendBytes, const std::string& path) const {
    if (maxFileBytes_ > 0 && appendBytes > maxFileBytes_)
        return true;
    if (maxTotalBytes_ > 0 && (totalBytesWritten_ + appendBytes) > maxTotalBytes_)
        return true;
    return false;
}

void ResultOutput::writeCsvToFile(const std::string& path) {
    std::ostringstream body;
    for (size_t i = 0; i < headers_.size(); ++i) {
        body << escapeCsvCell(headers_[i]);
        if (i < headers_.size() - 1) body << ",";
    }
    body << "\n";
    const size_t nc = headers_.size();
    for (const auto& row : rows_) {
        for (size_t i = 0; i < nc; ++i) {
            body << escapeCsvCell(i < row.size() ? row[i] : "");
            if (i < nc - 1) body << ",";
        }
        body << "\n";
    }
    std::string content = body.str();
    size_t len = content.size();
    if (wouldExceedLimits(len, path)) {
        lastFlushSkipped_ = true;
        TerminalMessage::error("[ResultOutput] CSV size limit exceeded. Skip writing: " + path);
        return;
    }
    std::ofstream f(path);
    if (!f) {
        lastFlushSkipped_ = true;
        TerminalMessage::error("[ResultOutput] Failed to open: " + path);
        return;
    }
    f << content;
    totalBytesWritten_ += len;
    lastFlushSkipped_ = false;
}

void ResultOutput::writeRowsAppend(const std::string& path) {
    std::ostringstream body;
    const size_t nc = headers_.size();
    for (const auto& row : rows_) {
        for (size_t i = 0; i < nc; ++i) {
            body << escapeCsvCell(i < row.size() ? row[i] : "");
            if (i < nc - 1) body << ",";
        }
        body << "\n";
    }
    std::string content = body.str();
    size_t len = content.size();
    if (len == 0) return;
    size_t existing = 0;
    std::ifstream in(path, std::ios::binary | std::ios::ate);
    if (in) existing = static_cast<size_t>(in.tellg());
    if (maxFileBytes_ > 0 && existing + len > maxFileBytes_) {
        lastFlushSkipped_ = true;
        TerminalMessage::error("[ResultOutput] CSV size limit exceeded. Skip appending: " + path);
        return;
    }
    if (maxTotalBytes_ > 0 && (totalBytesWritten_ + len) > maxTotalBytes_) {
        lastFlushSkipped_ = true;
        TerminalMessage::error("[ResultOutput] CSV total size limit exceeded. Skip appending: " + path);
        return;
    }
    std::ofstream f(path, std::ios::app);
    if (!f) {
        lastFlushSkipped_ = true;
        TerminalMessage::error("[ResultOutput] Failed to open for append: " + path);
        return;
    }
    f << content;
    totalBytesWritten_ += len;
    lastFlushSkipped_ = false;
}

void ResultOutput::setFilenameSame(const std::string& format) {
    filenameBefore_ = format;
    filenameAfter_ = format;
}

void ResultOutput::flush(ResultTiming timing) {
    if (!currentRow_.empty())
        rows_.push_back(std::move(currentRow_));
    currentRow_.clear();
    if (rows_.empty()) return;
    std::string path = (timing == ResultTiming::BeforeOptimization ? filenameBefore_ : filenameAfter_);
    if (path.empty()) return;
    path = replacePlaceholders(path, "");
    const bool sameFile = (path == lastPathWithHeader_);
    if (timing == ResultTiming::AfterOptimization && sameFile) {
        writeRowsAppend(path);
        lastPathWithHeader_.clear();
    } else {
        writeCsvToFile(path);
        if (timing == ResultTiming::BeforeOptimization)
            lastPathWithHeader_ = path;
        else
            lastPathWithHeader_.clear();
    }
    rows_.clear();
}

void ResultOutput::clear() {
    headers_.clear();
    rows_.clear();
    currentRow_.clear();
}

static double rmseFromResult(const ProductRunResult& r) {
    if (r.residuals.empty()) return 0.0;
    double sum = 0.0;
    for (double e : r.residuals) sum += e * e;
    return std::sqrt(sum / static_cast<double>(r.residuals.size()));
}

void ResultOutput::setPLOGFilename(const std::string& format) {
    plog_.setFilename(format);
}

void ResultOutput::PLOG_add(const std::string& column, const std::string& value, const std::string& /*format*/) {
    plog_.plogAdd(column, value);
}

void ResultOutput::PLOG_add(const std::string& column, double value, const std::string& format) {
    plog_.plogAdd(column, formatNumeric(value, format));
}

void ResultOutput::PLOG_add(const std::string& column, int value, const std::string& format) {
    plog_.plogAdd(column, formatNumeric(value, format));
}

void ResultOutput::PLOG_endRow() {
    plog_.plogEndRow();
    if (maxFileBytes_ > 0 && plog_.estimateBytes() >= maxFileBytes_)
        flushPLOG();
    else if (maxTotalBytes_ > 0 && (totalBytesWritten_ + plog_.estimateBytes()) >= maxTotalBytes_)
        flushPLOG();
}

void ResultOutput::writePLOG(const std::vector<ProductRunResult>& results, const std::string& column_name) {
    plog_.write(results, column_name);
}

void ResultOutput::flushPLOG() {
    if (plog_.getFilename().empty() || plog_.empty()) return;
    std::string path = replacePlaceholders(plog_.getFilename(), "");
    const auto& plogHeaders = plog_.getHeaders();
    const auto& plogRows = plog_.getRows();
    std::ostringstream body;
    for (size_t i = 0; i < plogHeaders.size(); ++i) {
        body << escapeCsvCell(plogHeaders[i]);
        if (i < plogHeaders.size() - 1) body << ",";
    }
    body << "\n";
    const size_t nc = plogHeaders.size();
    for (const auto& row : plogRows) {
        for (size_t i = 0; i < nc; ++i) {
            body << escapeCsvCell(i < row.size() ? row[i] : "");
            if (i < nc - 1) body << ",";
        }
        body << "\n";
    }
    std::string content = body.str();
    size_t len = content.size();
    if (wouldExceedLimits(len, path)) {
        lastFlushSkipped_ = true;
        TerminalMessage::error("[ResultOutput] PLOG size limit exceeded. Skip writing: " + path);
        return;
    }
    std::ofstream f(path);
    if (!f) {
        lastFlushSkipped_ = true;
        TerminalMessage::error("[ResultOutput] Failed to open PLOG: " + path);
        return;
    }
    f << content;
    totalBytesWritten_ += len;
    lastFlushSkipped_ = false;
    plog_.clear();
}

void ResultOutput::appendSummaryRows(const std::vector<ProductRunResult>& results) {
    for (const auto& r : results) {
        if (!r.ok) continue;
        addColumn("product_id", r.product_id);
        addColumn("rmse", rmseFromResult(r));
        endRow();
    }
}

// --- LLOG ---
static void ensureLogHeader(std::vector<std::string>& headers, std::vector<std::string>& currentRow, const std::string& header) {
    if (headers.empty() || std::find(headers.begin(), headers.end(), header) == headers.end())
        headers.push_back(header);
    size_t i = static_cast<size_t>(std::find(headers.begin(), headers.end(), header) - headers.begin());
    if (currentRow.size() <= i) currentRow.resize(i + 1, "");
}

void ResultOutput::setLLOGFilename(const std::string& format) { llogFilename_ = format; }

void ResultOutput::LLOG_add(const std::string& column, const std::string& value, const std::string& /*format*/) {
    ensureLogHeader(llogHeaders_, llogCurrentRow_, column);
    size_t i = static_cast<size_t>(std::find(llogHeaders_.begin(), llogHeaders_.end(), column) - llogHeaders_.begin());
    llogCurrentRow_[i] = value;
}

void ResultOutput::LLOG_add(const std::string& column, double value, const std::string& format) {
    ensureLogHeader(llogHeaders_, llogCurrentRow_, column);
    size_t i = static_cast<size_t>(std::find(llogHeaders_.begin(), llogHeaders_.end(), column) - llogHeaders_.begin());
    llogCurrentRow_[i] = formatNumeric(value, format);
}

void ResultOutput::LLOG_add(const std::string& column, int value, const std::string& format) {
    ensureLogHeader(llogHeaders_, llogCurrentRow_, column);
    size_t i = static_cast<size_t>(std::find(llogHeaders_.begin(), llogHeaders_.end(), column) - llogHeaders_.begin());
    llogCurrentRow_[i] = formatNumeric(value, format);
}

void ResultOutput::LLOG_endRow() {
    if (!llogCurrentRow_.empty()) {
        llogRows_.push_back(std::move(llogCurrentRow_));
        llogCurrentRow_.clear();
    }
    size_t est = 0;
    for (const auto& h : llogHeaders_) est += h.size() + 2;
    for (const auto& row : llogRows_) for (const auto& c : row) est += c.size() + 2;
    est += 200;
    if ((maxFileBytes_ > 0 && est >= maxFileBytes_) || (maxTotalBytes_ > 0 && (totalBytesWritten_ + est) >= maxTotalBytes_))
        flushLLOG();
}

void ResultOutput::flushLLOG() {
    if (llogFilename_.empty() || llogRows_.empty()) return;
    std::string path = replacePlaceholders(llogFilename_, "");
    std::ostringstream body;
    for (size_t i = 0; i < llogHeaders_.size(); ++i) {
        body << escapeCsvCell(llogHeaders_[i]);
        if (i < llogHeaders_.size() - 1) body << ",";
    }
    body << "\n";
    const size_t nc = llogHeaders_.size();
    for (const auto& row : llogRows_) {
        for (size_t i = 0; i < nc; ++i) {
            if (i) body << ",";
            body << escapeCsvCell(i < row.size() ? row[i] : "");
        }
        body << "\n";
    }
    std::string content = body.str();
    size_t len = content.size();
    if (wouldExceedLimits(len, path)) {
        lastFlushSkipped_ = true;
        TerminalMessage::error("[ResultOutput] LLOG size limit exceeded. Skip: " + path);
        return;
    }
    std::ofstream f(path);
    if (!f) {
        lastFlushSkipped_ = true;
        TerminalMessage::error("[ResultOutput] Failed to open LLOG: " + path);
        return;
    }
    f << content;
    totalBytesWritten_ += len;
    lastFlushSkipped_ = false;
    llogRows_.clear();
}

// --- DLOG ---
void ResultOutput::setDLOGFilename(const std::string& format) { dlogFilenameFormat_ = format; }

void ResultOutput::DLOG_beginProduct(const std::string& product_id) {
    dlogCurrentProductId_ = product_id;
    dlogCurrentPath_ = replacePlaceholders(dlogFilenameFormat_, product_id);
    if (dlogFile_.is_open()) dlogFile_.close();
    dlogCurrentRow_.clear();
}

void ResultOutput::DLOG_add(const std::string& column, const std::string& value, const std::string& /*format*/) {
    ensureLogHeader(dlogHeaders_, dlogCurrentRow_, column);
    size_t i = static_cast<size_t>(std::find(dlogHeaders_.begin(), dlogHeaders_.end(), column) - dlogHeaders_.begin());
    dlogCurrentRow_[i] = value;
}

void ResultOutput::DLOG_add(const std::string& column, double value, const std::string& format) {
    ensureLogHeader(dlogHeaders_, dlogCurrentRow_, column);
    size_t i = static_cast<size_t>(std::find(dlogHeaders_.begin(), dlogHeaders_.end(), column) - dlogHeaders_.begin());
    dlogCurrentRow_[i] = formatNumeric(value, format);
}

void ResultOutput::DLOG_add(const std::string& column, int value, const std::string& format) {
    ensureLogHeader(dlogHeaders_, dlogCurrentRow_, column);
    size_t i = static_cast<size_t>(std::find(dlogHeaders_.begin(), dlogHeaders_.end(), column) - dlogHeaders_.begin());
    dlogCurrentRow_[i] = formatNumeric(value, format);
}

void ResultOutput::DLOG_endRow() {
    if (dlogCurrentPath_.empty()) return;
    if (!dlogFile_.is_open()) {
        dlogFile_.open(dlogCurrentPath_);
        if (!dlogFile_) {
            TerminalMessage::error("[ResultOutput] Failed to open DLOG: " + dlogCurrentPath_);
            return;
        }
        for (size_t i = 0; i < dlogHeaders_.size(); ++i) {
            dlogFile_ << escapeCsvCell(dlogHeaders_[i]);
            if (i < dlogHeaders_.size() - 1) dlogFile_ << ",";
        }
        dlogFile_ << "\n";
    }
    const size_t nc = dlogHeaders_.size();
    for (size_t i = 0; i < nc; ++i)
        dlogFile_ << escapeCsvCell(i < dlogCurrentRow_.size() ? dlogCurrentRow_[i] : "");
    for (size_t i = dlogCurrentRow_.size(); i < nc; ++i) dlogFile_ << ",";
    dlogFile_ << "\n";
    dlogFile_.flush();
    totalBytesWritten_ += 100;  // 見込み
    dlogCurrentRow_.clear();
}

void ResultOutput::writeLLOG(const std::vector<ProductRunResult>& results,
                             size_t startIndex,
                             size_t maxPoints,
                             const std::string& filenameFormat) {
    writeDetailCsv(results, startIndex, maxPoints, true, filenameFormat);
}

void ResultOutput::writeDLOG(const std::vector<ProductRunResult>& results,
                             size_t startIndex,
                             size_t maxPoints,
                             const std::string& filenameFormat) {
    writeDetailCsv(results, startIndex, maxPoints, false, filenameFormat, nullptr);
}

void ResultOutput::writeDLOG(const std::vector<ProductRunResult>& results,
                             const std::string& filenameFormat,
                             const std::vector<std::pair<size_t, size_t>>& perProductRanges) {
    if (perProductRanges.size() != results.size()) return;
    writeDetailCsv(results, 0, 0, false, filenameFormat, &perProductRanges);
}

void ResultOutput::writeDetailCsv(const std::vector<ProductRunResult>& results,
                                  size_t startIndex,
                                  size_t maxPoints,
                                  bool oneFile,
                                  const std::string& filenameFormat,
                                  const std::vector<std::pair<size_t, size_t>>* perProductRanges) {
    if (filenameFormat.empty()) return;
    std::vector<std::string> detailHeaders = {"product_id", "point_index", "measured", "predicted", "residual"};
    std::vector<std::string> extraNames;
    for (const auto& r : results)
        for (const auto& p : r.extra_columns)
            extraNames.push_back(p.first);
    std::sort(extraNames.begin(), extraNames.end());
    extraNames.erase(std::unique(extraNames.begin(), extraNames.end()), extraNames.end());
    for (const auto& n : extraNames)
        detailHeaders.push_back(n);

    auto writeOneTable = [&](const std::string& path, size_t resultStart, size_t resultCount) {
        std::vector<std::vector<std::string>> table;
        for (size_t ri = resultStart; ri < resultStart + resultCount && ri < results.size(); ++ri) {
            const auto& r = results[ri];
            size_t n = r.measured.size();
            size_t segStart = startIndex;
            size_t segPoints = maxPoints;
            if (perProductRanges && !oneFile && resultCount == 1 && ri < perProductRanges->size()) {
                segStart = (*perProductRanges)[ri].first;
                segPoints = (*perProductRanges)[ri].second;
            }
            for (size_t k = 0; k < segPoints; ++k) {
                std::vector<std::string> row;
                row.push_back(r.product_id);
                row.push_back(std::to_string(static_cast<int>(k)));
                if (k < n && segStart + k < n) {
                    size_t idx = segStart + k;
                    row.push_back(formatNumeric(r.measured[idx], ""));
                    row.push_back(formatNumeric(r.predicted[idx], ""));
                    row.push_back(formatNumeric(r.residuals[idx], ""));
                } else {
                    row.push_back("");
                    row.push_back("");
                    row.push_back("");
                }
                for (size_t h = 5; h < detailHeaders.size(); ++h) {
                    std::string val;
                    for (const auto& p : r.extra_columns)
                        if (p.first == detailHeaders[h]) { val = p.second; break; }
                    row.push_back(val);
                }
                table.push_back(std::move(row));
            }
        }
        size_t approxBytes = 0;
        for (const auto& row : table)
            for (const auto& cell : row) approxBytes += cell.size() + 2;
        approxBytes += 200;
        if (wouldExceedLimits(approxBytes, path)) {
            TerminalMessage::error("[ResultOutput] Detail CSV size limit exceeded. Skip: " + path);
            return;
        }
        std::ofstream f(path);
        if (!f) {
            TerminalMessage::error("[ResultOutput] Failed to open: " + path);
            return;
        }
        for (size_t i = 0; i < detailHeaders.size(); ++i) {
            f << escapeCsvCell(detailHeaders[i]);
            if (i < detailHeaders.size() - 1) f << ",";
        }
        f << "\n";
        for (const auto& row : table) {
            for (size_t i = 0; i < row.size(); ++i) {
                f << escapeCsvCell(row[i]);
                if (i < row.size() - 1) f << ",";
            }
            f << "\n";
        }
        totalBytesWritten_ += approxBytes;
    };

    if (oneFile) {
        std::string path = replacePlaceholders(filenameFormat, "");
        writeOneTable(path, 0, results.size());
    } else {
        for (size_t i = 0; i < results.size(); ++i) {
            std::string path = replacePlaceholders(filenameFormat, results[i].product_id);
            writeOneTable(path, i, 1);
        }
    }
}

}  // namespace optimizer
