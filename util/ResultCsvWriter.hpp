/**
 * @file ResultCsvWriter.hpp
 * @brief ProductRunResult のベクトルを CSV に書き出す。csvwriter の writeCsvTable を利用。
 * 各製品の extra_columns をそのまま列として出力する。
 */

#ifndef OPTIMIZER_UTIL_RESULT_CSV_WRITER_H
#define OPTIMIZER_UTIL_RESULT_CSV_WRITER_H

#include "util/csvwriter.hpp"
#include "product/ProductRunResult.hpp"
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace optimizer {

/**
 * @brief ProductRunResult のベクトルを CSV に書き出す
 *
 * 列: product_id, ok, error_message, (extra_columns の名前…), n_points, rmse, mean_residual,
 *     measured_0, predicted_0, residuals_0, measured_1, ... （最大 maxDataCols 点まで）
 *
 * @param filename 出力 CSV パス
 * @param results 製品ごとの計算結果（IResultWriter に渡されるものと同じ）
 * @param maxDataCols 1製品あたりの measured/predicted/residuals を何点まで列にするか（0 で省略）
 */
inline void writeProductResultCsv(const std::string& filename,
                                  const std::vector<ProductRunResult>& results,
                                  size_t maxDataCols = 64) {
    if (results.empty()) {
        ::writeCsvTable(filename, {"product_id"}, {});
        return;
    }

    std::vector<std::string> headers = {"product_id", "ok", "error_message"};

    std::vector<std::string> extraNames;
    for (const auto& r : results) {
        for (const auto& p : r.extra_columns)
            extraNames.push_back(p.first);
    }
    std::sort(extraNames.begin(), extraNames.end());
    extraNames.erase(std::unique(extraNames.begin(), extraNames.end()), extraNames.end());
    for (const auto& n : extraNames)
        headers.push_back(n);

    headers.push_back("n_points");
    headers.push_back("rmse");
    headers.push_back("mean_residual");

    for (size_t k = 0; k < maxDataCols; ++k) {
        headers.push_back("measured_" + std::to_string(k));
        headers.push_back("predicted_" + std::to_string(k));
        headers.push_back("residuals_" + std::to_string(k));
    }

    std::vector<std::vector<std::string>> rows;
    for (const auto& r : results) {
        std::vector<std::string> row;
        row.push_back(r.product_id);
        row.push_back(r.ok ? "1" : "0");
        row.push_back(r.error_message);
        for (const auto& name : extraNames) {
            std::string val;
            for (const auto& p : r.extra_columns) {
                if (p.first == name) { val = p.second; break; }
            }
            row.push_back(val);
        }
        const size_t n = r.residuals.size();
        double rmse = 0.0, meanRes = 0.0;
        if (n > 0) {
            for (size_t i = 0; i < n; ++i) {
                meanRes += r.residuals[i];
                rmse += r.residuals[i] * r.residuals[i];
            }
            meanRes /= static_cast<double>(n);
            rmse = std::sqrt(rmse / static_cast<double>(n));
        }
        row.push_back(std::to_string(n));
        row.push_back(std::to_string(rmse));
        row.push_back(std::to_string(meanRes));
        for (size_t k = 0; k < maxDataCols; ++k) {
            if (k < n) {
                row.push_back(std::to_string(r.measured[k]));
                row.push_back(std::to_string(r.predicted[k]));
                row.push_back(std::to_string(r.residuals[k]));
            } else {
                row.push_back("");
                row.push_back("");
                row.push_back("");
            }
        }
        rows.push_back(std::move(row));
    }

    ::writeCsvTable(filename, headers, rows);
}

}  // namespace optimizer

#endif
