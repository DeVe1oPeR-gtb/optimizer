#ifndef D_OPTIMIZER_H
#define D_OPTIMIZER_H

#include <cmath>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
//
//  Optimizer is responsible for ...
//
///////////////////////////////////////////////////////////////////////////////
// 
class Optimizer {
  public:
    explicit Optimizer();
    virtual ~Optimizer();

    template <typename T>
    struct Stats {
        T mean;
        T std_dev;
        T rmse;

        void initialize() {
            mean    = static_cast<T>(0.0);
            std_dev = static_cast<T>(0.0);
            rmse    = static_cast<T>(0.0);
        }
    };

    template <typename T>
    Stats<T> computeStats(const std::vector<std::pair<T, T>>& eval_data) {
      Stats<T> stats;
      stats.initialize();
  
      if (eval_data.empty()) {
          return stats;
      }
  
      std::vector<T> diffs;
      diffs.reserve(eval_data.size());
  
      // 差分を計算
      for (const auto &pair : eval_data) {
          diffs.push_back(pair.first - pair.second);
      }
  
      // 平均を計算
      stats.mean = std::accumulate(diffs.begin(), diffs.end(), static_cast<T>(0.0)) / diffs.size();
  
      // 分散（標準偏差用）
      T variance          = 0.0;
      T squared_error_sum = 0.0;
      for (T diff : diffs) {
          variance += (diff - stats.mean) * (diff - stats.mean);
          squared_error_sum += diff * diff;
      }
      variance /= diffs.size();
      stats.std_dev = std::sqrt(variance);
  
      // RMSE 計算
      stats.rmse = std::sqrt(squared_error_sum / diffs.size());
  
      return stats;
    }

    virtual void setParam(const std::vector<double> &) {}

  private:
    Optimizer(const Optimizer &)            = delete;
    Optimizer &operator=(const Optimizer &) = delete;
};

#endif // D_OPTIMIZER_H
