#ifndef OPTIMIZER_UTIL_TRACE_CONFIG_H
#define OPTIMIZER_UTIL_TRACE_CONFIG_H

#include <iosfwd>
#include <string>
#include <vector>

namespace optimizer {

/**
 * @brief 開発者用設定の読込
 *
 * config/developer.cfg を読み、trace=on|off と optimizer=PSO,DE,LM を解釈する。
 */
class TraceConfig {
public:
    /** @brief 設定ファイルを読み、トレース・最適化器リストを更新 */
    static void load(const std::string& path = "config/developer.cfg");

    /** @brief トレース出力が有効か */
    static bool isTraceEnabled();

    /** @brief 実行する最適化器名のリスト（未指定時は PSO, DE, LM） */
    static const std::vector<std::string>& getOptimizersToRun();

    /** @brief トレース出力先（デフォルトは nullptr = 無効）。caller が setTraceStream で設定。 */
    static std::ostream* getTraceStream();
    static void setTraceStream(std::ostream* s);

private:
    static bool traceEnabled_;
    static std::ostream* traceStream_;
    static std::vector<std::string> optimizersToRun_;
};

}  // namespace optimizer

#endif  // OPTIMIZER_UTIL_TRACE_CONFIG_H
