/**
 * @file Handler.cpp
 * @brief 設定ファイルを読んで、実行する最適化器名のリスト（optimizer=PSO,DE,LM）を返す。デモ用。
 */

#include "util/util_common.hpp"
#include "util/ParaConfig.hpp"

Handler::Handler(const std::string& configPath) {
    optimizer::ParaConfig::load(configPath);
}

Handler::~Handler() {}

std::vector<std::string> Handler::getOptimizersToRun() const {
    return optimizer::ParaConfig::getOptimizersToRun();
}
