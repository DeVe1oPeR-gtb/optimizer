#ifndef OPTIMIZER_UTIL_OPTIMIZER_DRIVER_H
#define OPTIMIZER_UTIL_OPTIMIZER_DRIVER_H

/**
 * @file OptimizerDriver.h
 * @brief 最適化 1 回分の実行窓口。run(config, mapper, model, loader, products, optimizerName, ...) で PSO/DE/LM を実行。
 */

#include "param/ParameterMapper.hpp"
#include "product/ProductMeta.hpp"
#include "util/RunConfig.hpp"
#include <string>
#include <vector>

namespace optimizer {

class IPhysicalModel;
class IProductDataLoader;
class IResultWriter;

/**
 * @brief 1 回分の最適化結果（最良スコアと最良パラメータベクトル）
 */
struct RunResult {
    double bestScore = 0.0;
    std::vector<double> bestParams;
};

/**
 * @brief 最適化 1 回分の実行窓口（現場は model / loader / mapper / products を渡して run するだけ）
 *
 * 設定はファイルパスまたは RunConfig で渡す。PSO/DE/LM の詳細はドライバ内に閉じる。
 * resultWriter を渡すと、最適化終了後に最適化済みパラメータで各製品を再計算し、writeAfterOptimization を呼ぶ。
 */
class OptimizerDriver {
public:
    /** @brief 設定ファイルパスで 1 回実行。mapper は呼び出し側で 1 本用意したものを渡す。 */
    static RunResult run(const std::string& configPath,
                         ParameterMapper& mapper,
                         IPhysicalModel& model,
                         IProductDataLoader& loader,
                         const std::vector<ProductMeta>& products,
                         const std::string& optimizerName,
                         const std::string& tracePath = "",
                         const char* logLabel = nullptr,
                         IResultWriter* resultWriter = nullptr);

    /** @brief RunConfig で設定を注入して 1 回実行（ファイルに依存しない） */
    static RunResult run(const RunConfig& config,
                         ParameterMapper& mapper,
                         IPhysicalModel& model,
                         IProductDataLoader& loader,
                         const std::vector<ProductMeta>& products,
                         const std::string& optimizerName,
                         const std::string& tracePath = "",
                         const char* logLabel = nullptr,
                         IResultWriter* resultWriter = nullptr);

    /**
     * @brief 最適化なしで適用値のみ計算し、結果を出力する
     *
     * DB から読んだ適用値と設定の初期値で fullParams を組み、各製品を 1 回ずつ計算して
     * resultWriter.writeApplyOnly を呼ぶ。
     */
    static void runApplyOnly(ParameterMapper& mapper,
                             IPhysicalModel& model,
                             IProductDataLoader& loader,
                             const std::vector<ProductMeta>& products,
                             DbValueProvider dbValueProvider,
                             IResultWriter& resultWriter);
};

}  // namespace optimizer

#endif
