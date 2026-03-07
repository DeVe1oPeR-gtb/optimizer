#ifndef OPTIMIZER_UTIL_HANDLER_H
#define OPTIMIZER_UTIL_HANDLER_H

#include <string>
#include <vector>

/**
 * @brief 設定ファイルで指定された最適化器を返す窓口
 *
 * コンストラクタで config を読込み、getOptimizersToRun() で実行する最適化器名のリストを取得する。
 */
class Handler {
public:
    explicit Handler(const std::string& configPath);
    virtual ~Handler();

    /** @brief 実行する最適化器名のリスト（設定の optimizer= で指定、未指定時は PSO,DE,LM） */
    std::vector<std::string> getOptimizersToRun() const;
};

#endif
