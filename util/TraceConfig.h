#ifndef OPTIMIZER_UTIL_TRACE_CONFIG_H
#define OPTIMIZER_UTIL_TRACE_CONFIG_H

#include <iosfwd>
#include <string>

namespace optimizer {

/**
 * @brief 開発者用設定（トレースON/OFF）の読込
 *
 * config/developer.cfg を読み、trace=on|off でトレース出力を制御する。
 */
class TraceConfig {
public:
    /** @brief 設定ファイルを読み、トレース有効フラグを更新 */
    static void load(const std::string& path = "config/developer.cfg");

    /** @brief トレース出力が有効か */
    static bool isTraceEnabled();

    /** @brief トレース出力先（デフォルトは nullptr = 無効）。caller が setTraceStream で設定。 */
    static std::ostream* getTraceStream();
    static void setTraceStream(std::ostream* s);

private:
    static bool traceEnabled_;
    static std::ostream* traceStream_;
};

}  // namespace optimizer

#endif  // OPTIMIZER_UTIL_TRACE_CONFIG_H
