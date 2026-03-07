#ifndef D_OPTIMIZER_H
#define D_OPTIMIZER_H

#include <cmath>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

/**
 * @brief 最適化アルゴリズムの基底クラス
 *
 * 評価データ（実測・予測ペア）からの統計計算を提供する。
 */
class Optimizer {
  public:
    explicit Optimizer();
    virtual ~Optimizer();

    /**
     * @brief 評価データの統計（平均・標準偏差・RMSE）
     */
    template <typename T>
    struct Stats {
        T mean;     /**< 平均 */
        T std_dev;  /**< 標準偏差 */
        T rmse;     /**< 二乗平均平方根誤差 */

        void initialize() {
            mean    = static_cast<T>(0.0);
            std_dev = static_cast<T>(0.0);
            rmse    = static_cast<T>(0.0);
        }
    };

    /**
     * @brief 実測・予測ペア列から統計を計算する
     * @param eval_data 各要素が (実測値, 予測値) のペア
     * @return 差分の平均・標準偏差・RMSE
     */
    template <typename T>
    Stats<T> computeStats(const std::vector<std::pair<T, T>>& eval_data) {
      Stats<T> stats;
      stats.initialize();

      if (eval_data.empty()) {
          return stats;
      }

      std::vector<T> diffs;
      diffs.reserve(eval_data.size());

      for (const auto &pair : eval_data) {
          diffs.push_back(pair.first - pair.second);
      }

      stats.mean = std::accumulate(diffs.begin(), diffs.end(), static_cast<T>(0.0)) / diffs.size();

      T variance          = 0.0;
      T squared_error_sum = 0.0;
      for (T diff : diffs) {
          variance += (diff - stats.mean) * (diff - stats.mean);
          squared_error_sum += diff * diff;
      }
      variance /= diffs.size();
      stats.std_dev = std::sqrt(variance);
      stats.rmse = std::sqrt(squared_error_sum / diffs.size());

      return stats;
    }

    /** @brief パラメータ設定用（派生でオーバーライド可能） */
    virtual void setParam(const std::vector<double> &) {}

  private:
    Optimizer(const Optimizer &)            = delete;
    Optimizer &operator=(const Optimizer &) = delete;
};

#endif // D_OPTIMIZER_H
