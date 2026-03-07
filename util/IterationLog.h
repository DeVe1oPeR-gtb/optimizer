#ifndef OPTIMIZER_UTIL_ITERATION_LOG_H
#define OPTIMIZER_UTIL_ITERATION_LOG_H

#include <vector>
#include <iosfwd>

namespace optimizer {

/**
 * @brief 最適化の反復1回分をターミナル（標準出力）へ出力する
 *
 * フォーマットは util/IterationLog.cpp 内で変更可能。
 * @param iteration 反復番号（0始まり）
 * @param score 現在の最良目的値（RMSE等）
 * @param position 現在の最良パラメータベクトル
 * @param label ラベル（例: "PSO", "Model1-DE"）。nullptr の場合は省略
 */
void logIteration(int iteration, double score,
                  const std::vector<double>& position,
                  const char* label = nullptr);

/**
 * @brief 上記と同様だが出力先を指定する版
 */
void logIterationTo(int iteration, double score,
                    const std::vector<double>& position,
                    std::ostream& out,
                    const char* label = nullptr);

}  // namespace optimizer

#endif  // OPTIMIZER_UTIL_ITERATION_LOG_H
