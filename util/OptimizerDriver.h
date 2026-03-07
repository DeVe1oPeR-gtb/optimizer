#ifndef OPTIMIZER_UTIL_OPTIMIZER_DRIVER_H
#define OPTIMIZER_UTIL_OPTIMIZER_DRIVER_H

#include "param/ParameterMapper.h"
#include "product/ProductMeta.h"
#include "util/RunConfig.h"
#include <string>
#include <vector>

namespace optimizer {

class IPhysicalModel;
class IProductDataLoader;

/**
 * @brief 1 回分の最適化結果
 */
struct RunResult {
    double bestScore = 0.0;
    std::vector<double> bestParams;
};

/**
 * @brief 最適化 1 回分の実行窓口（現場は model / loader / mapper / products を渡して run するだけ）
 *
 * 設定はファイルパスまたは RunConfig で渡す。PSO/DE/LM の詳細はドライバ内に閉じる。
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
                         const char* logLabel = nullptr);

    /** @brief RunConfig で設定を注入して 1 回実行（ファイルに依存しない） */
    static RunResult run(const RunConfig& config,
                         ParameterMapper& mapper,
                         IPhysicalModel& model,
                         IProductDataLoader& loader,
                         const std::vector<ProductMeta>& products,
                         const std::string& optimizerName,
                         const std::string& tracePath = "",
                         const char* logLabel = nullptr);
};

}  // namespace optimizer

#endif
