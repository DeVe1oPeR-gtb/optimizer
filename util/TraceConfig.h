#ifndef OPTIMIZER_UTIL_TRACE_CONFIG_H
#define OPTIMIZER_UTIL_TRACE_CONFIG_H

/**
 * @file TraceConfig.h
 * @brief 開発者用設定（trace, optimizer, lm_apply_bounds）の読込と取得。
 */

#include "util/RunConfig.h"
#include <iosfwd>
#include <string>
#include <vector>

namespace optimizer {

/**
 * @brief 開発者用設定の読込
 *
 * config/developer.cfg を読み、trace=on|off, optimizer=PSO,DE,LM, lm_apply_bounds=on|off を解釈する。
 * RunConfig を渡せばファイルに依存せず設定を注入できる。
 */
class TraceConfig {
public:
    /** @brief 設定ファイルを読み、トレース・最適化器リストを更新 */
    static void load(const std::string& path = "config/developer.cfg");

    /** @brief RunConfig の内容で設定を上書き（ファイルを読まない） */
    static void loadFromStruct(const RunConfig& config);

    /** @brief トレース出力が有効か */
    static bool isTraceEnabled();

    /** @brief LM でパラメータ上下限を適用するか（developer.cfg の lm_apply_bounds） */
    static bool isLmApplyBoundsEnabled();

    /** @brief 実行する最適化器名のリスト（未指定時は PSO, DE, LM） */
    static const std::vector<std::string>& getOptimizersToRun();

    /** @brief トレース出力先（デフォルトは nullptr = 無効）。caller が setTraceStream で設定。 */
    static std::ostream* getTraceStream();
    static void setTraceStream(std::ostream* s);

private:
    static bool traceEnabled_;
    static std::ostream* traceStream_;
    static std::vector<std::string> optimizersToRun_;
    static bool lmApplyBoundsEnabled_;
};

}  // namespace optimizer

#endif  // OPTIMIZER_UTIL_TRACE_CONFIG_H
