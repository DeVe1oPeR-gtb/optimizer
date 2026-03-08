/**
 * @file Handler.cpp
 * @brief 設定ファイルを読んで、実行する最適化器名のリスト（optimizer=PSO,DE,LM）を返す。デモ用。
 */

#include "util/util_common.hpp"
#include "util/ParaConfig.hpp"

Handler::Handler(const std::string& configPath) {
    // デモ/オンサイトで optimizer= を読むため、起動時に para.cfg を ParaConfig に読ませる。
    optimizer::ParaConfig::load(configPath);
}

Handler::~Handler() {}

std::vector<std::string> Handler::getOptimizersToRun() const {
    // 設定はコンストラクタで load 済み。PSO/DE/LM のリストをそのまま返す。
    return optimizer::ParaConfig::getOptimizersToRun();
}
