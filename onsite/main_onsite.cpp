/*
 * オンサイト用メイン。このファイルだけ編集し、scripts/setup_onsite.sh 実行後に make onsite でビルド・実行する。
 *
 * 【現地で作成する関数・クラス】いずれもこのファイル内にテンプレートあり。該当ブロックを実装で置き換える。
 *   1. makeSpecs()        — 最適化パラメータ仕様（CSV/現場設定から ParamSpec を返す）
 *   2. makeProducts()      — 製品メタのリスト（コイル一覧などから ProductMeta の列を返す）
 *   3. OnsiteModel         — IPhysicalModel の実装（現場の物理モデル 1 回実行）
 *   4. OnsiteLoader        — IProductDataLoader の実装（1 製品分のデータ読込）
 *   5. OnsiteResultWriter  — （任意）IResultWriter の実装（適用のみ／最適化後の結果をファイル・DBへ出力）
 */

#include "param/param.hpp"
#include "util/util_common.hpp"
#include "util/TraceConfig.hpp"
#include "util/LogRotate.hpp"
#include "util/OptimizerDriver.hpp"
#include "util/IResultWriter.hpp"
#include "model/IPhysicalModel.hpp"
#include "model/IProductDataLoader.hpp"
#include "product/ProductMeta.hpp"
#include "product/ProductLoadedData.hpp"
#include "product/ProductRunResult.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include <sys/stat.h>

namespace {

static void ensureLogDir() {
#ifdef _WIN32
    _mkdir("log");
#else
    mkdir("log", 0755);
#endif
}

// =============================================================================
// 1. makeSpecs() — 最適化するパラメータの仕様
// =============================================================================
// 戻り値: ParamSpec の列。
// ParamSpec: param_name, enable_opt(1=最適化に含める), init_mode(Manual/Db/Hybrid),
//            init_value, db_key, lower, upper, note, apply_bounds.
// CSV から読む例: CsvParamLoader().load("config/params.csv", err) で std::vector<ParamSpec> 取得。
/** 現地実装: CSV または現場設定から ParamSpec を組み立てる */
static std::vector<optimizer::ParamSpec> makeSpecs() {
    // ----- テンプレート: 以下を現場仕様に書き換える -----
    return {
        {"p0", 1, optimizer::InitMode::Manual, 1.0, "", -5.0, 5.0, ""},
        {"p1", 1, optimizer::InitMode::Manual, -0.5, "", -5.0, 5.0, ""},
        {"p2", 1, optimizer::InitMode::Manual, 0.1, "", -5.0, 5.0, ""},
    };
}

// =============================================================================
// 2. makeProducts() — 製品メタのリスト
// =============================================================================
// 戻り値: 最適化対象とする製品の ProductMeta の列。
// ProductMeta: product_id, file_path, excluded_data_indices(対象外データ点の 0 始まりインデックス、空で全点対象)。
/** 現地実装: 現場のコイル/製品一覧から ProductMeta を組み立てる */
static std::vector<optimizer::ProductMeta> makeProducts() {
    // ----- テンプレート: 以下を現場仕様に書き換える -----
    return {{"product1", ""}};
}

// =============================================================================
// 3. OnsiteModel — IPhysicalModel の実装（1 製品分のモデル 1 回実行）
// =============================================================================
// run(fullParams, productLoadedData): fullParams は最適化ベクトル＋固定パラメータの並び。
// productLoadedData は OnsiteLoader::load() が返した ProductLoadedData* を void* で受け取る。
// 戻り値: 実測と同じ長さ・同じ位置順の予測値の列（残差 = measured - predicted）。
/** 現地実装: 現場の物理モデルで run() を実装する（または compat_model を include して差し替え） */
class OnsiteModel : public optimizer::IPhysicalModel {
public:
    std::vector<double> run(const std::vector<double>& fullParams,
                            const void* productLoadedData) override {
        // ----- テンプレート: 以下を現場の物理モデルに書き換える -----
        const auto* data = static_cast<const optimizer::ProductLoadedData*>(productLoadedData);
        if (!data || data->positions.empty()) return {};
        std::vector<double> pred(data->positions.size());
        for (size_t i = 0; i < data->positions.size(); ++i) {
            double x = data->positions[i];
            pred[i] = fullParams[0] + fullParams[1] * x + fullParams[2] * x * x;
        }
        return pred;
    }
};

// =============================================================================
// 4. OnsiteLoader — IProductDataLoader の実装（1 製品分のデータ読込）
// =============================================================================
// load(meta): 指定製品の実測・位置等をロードし ProductLoadedData を返す。失敗時は nullptr。
// ProductLoadedData: measured, positions を 1:1 対応でセットする。
/** 現地実装: 現場のファイル/DB から 1 製品分をロードする（または compat_data を include して差し替え） */
class OnsiteLoader : public optimizer::IProductDataLoader {
public:
    std::unique_ptr<optimizer::ProductLoadedData> load(const optimizer::ProductMeta& meta) override {
        // ----- テンプレート: 以下を現場の読込仕様に書き換える -----
        auto data = std::make_unique<optimizer::ProductLoadedData>();
        data->measured = {1.0 - 0.5 * 0.5 + 0.1 * 0.25, 1.0 - 0.5 * 1.0 + 0.1 * 1.0, 1.0 - 0.5 * 1.5 + 0.1 * 2.25};
        data->positions = {0.5, 1.0, 1.5};
        return data;
    }
};

// =============================================================================
// 5. OnsiteResultWriter — IResultWriter の実装（任意）
// =============================================================================
// 適用値のみ計算: writeApplyOnly(fullParams, results)
// 最適化終了後: writeAfterOptimization(fullParams, results)
// 使う場合: OptimizerDriver::run(..., &writer) または runApplyOnly(..., writer) に渡す。
/** 現地実装: 結果をファイル・DB 等へ出力する場合に実装する */
class OnsiteResultWriter : public optimizer::IResultWriter {
public:
    void writeApplyOnly(const std::vector<double>& fullParams,
                        const std::vector<optimizer::ProductRunResult>& results) override {
        // ----- テンプレート: 適用値のみ計算した結果を出力する -----
        (void)fullParams;
        (void)results;
    }

    void writeAfterOptimization(const std::vector<double>& fullParams,
                                const std::vector<optimizer::ProductRunResult>& results) override {
        // ----- テンプレート: 最適化後の結果を出力する -----
        (void)fullParams;
        (void)results;
    }
};

}  // namespace

int main() {
    const std::string configPath = "config/para.cfg";

    Handler handler(configPath);
    optimizer::DataConfig::load(configPath);

    if (optimizer::TraceConfig::isTraceEnabled() || optimizer::TraceConfig::isDebugEnabled())
        ensureLogDir();
    static std::ofstream s_debugLog;
    if (optimizer::TraceConfig::isDebugEnabled()) {
        if (optimizer::openLogWithRotation("log/debug.log", s_debugLog, optimizer::TraceConfig::getDebugLogMaxBytes()))
            optimizer::TraceConfig::setDebugStream(&s_debugLog);
    }

    optimizer::ParameterMapper mapper;
    mapper.setSpecs(makeSpecs());
    std::string err;
    if (!mapper.validate(err)) {
        optimizer::TerminalMessage::error("ParameterMapper: " + err);
        return 1;
    }

    OnsiteModel model;
    OnsiteLoader loader;
    std::vector<optimizer::ProductMeta> products = makeProducts();

    // 結果をファイル/DB へ出す場合は OnsiteResultWriter を渡す:
    //   OnsiteResultWriter resultWriter;
    //   result = optimizer::OptimizerDriver::run(..., tracePath, "onsite", &resultWriter);
    const std::string optimizerName = handler.getOptimizersToRun().empty()
                                          ? "PSO"
                                          : handler.getOptimizersToRun()[0];
    std::string tracePath = optimizer::TraceConfig::isTraceEnabled() ? "log/onsite_trace.csv" : "";
    optimizer::RunResult result = optimizer::OptimizerDriver::run(
        configPath, mapper, model, loader, products, optimizerName,
        tracePath, "onsite", nullptr);

    std::cout << "[Onsite] optimizer=" << optimizerName
              << " bestScore=" << result.bestScore
              << " bestParams=";
    for (size_t i = 0; i < result.bestParams.size(); ++i)
        std::cout << (i ? "," : "") << result.bestParams[i];
    std::cout << "\n";
    return 0;
}
