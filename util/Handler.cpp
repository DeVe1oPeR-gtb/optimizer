/**
 * @file Handler.cpp
 * @brief 設定ファイルを読んで、実行する最適化器名のリスト（optimizer=PSO,DE,LM）を返す。デモ用。
 */

#include "util/Handler.h"
#include "util/TraceConfig.h"

Handler::Handler(const std::string& configPath) {
    optimizer::TraceConfig::load(configPath);
}

Handler::~Handler() {}

std::vector<std::string> Handler::getOptimizersToRun() const {
    return optimizer::TraceConfig::getOptimizersToRun();
}
