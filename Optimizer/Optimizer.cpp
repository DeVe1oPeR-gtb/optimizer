#include "Optimizer.h"

Optimizer::Optimizer() {}

Optimizer::~Optimizer() {}

Optimizer::Stats Optimizer::computeStats(const std::vector<std::pair<double, double>>& eval_data) {
    Stats stats;
    stats.initialize();

    if (eval_data.empty()) {
        return stats;
    }

    std::vector<double> diffs;
    diffs.reserve(eval_data.size());

    // 差分を計算
    for (const auto &pair : eval_data) {
        diffs.push_back(pair.first - pair.second);
    }

    // 平均を計算
    stats.mean = std::accumulate(diffs.begin(), diffs.end(), 0.0) / diffs.size();

    // 分散（標準偏差用）
    double variance          = 0.0;
    double squared_error_sum = 0.0;
    for (double diff : diffs) {
        variance += (diff - stats.mean) * (diff - stats.mean);
        squared_error_sum += diff * diff;
    }
    variance /= diffs.size();
    stats.std_dev = std::sqrt(variance);

    // RMSE 計算
    stats.rmse = std::sqrt(squared_error_sum / diffs.size());

    return stats;
}
