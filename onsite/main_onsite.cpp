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
#include "util/ParaConfig.hpp"
#include "util/LogRotate.hpp"
#include "util/OptimizerDriver.hpp"
#include "util/IResultWriter.hpp"
#include "util/ResultCsvWriter.hpp"
#include "util/ResultOutput.hpp"
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

static void ensureResultDir() {
#ifdef _WIN32
    _mkdir("result");
#else
    mkdir("result", 0755);
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
// productLoadedData は Loader で cfg に従い選択・連結済みの measured/positions（1:1 対応）。
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
// position は 0~1 で入れる。cfg の optimization_position_min/max（例: 0.05, 0.95）の範囲外は最適化対象から外す。
// 製品ごとに複数種類ある場合は data_sets に積み、selectAndConcatDataSets → filterByPositionRange の順で適用。
/** 現地実装: 現場のファイル/DB から 1 製品分をロードする（または compat_data を include して差し替え） */
class OnsiteLoader : public optimizer::IProductDataLoader {
public:
    std::unique_ptr<optimizer::ProductLoadedData> load(const optimizer::ProductMeta& meta) override {
        // ----- テンプレート: 以下を現場の読込仕様に書き換える -----
        auto data = std::make_unique<optimizer::ProductLoadedData>();

        // パターンA: 1 種類だけのときは measured/positions を直接セット（position は 0~1）
        data->measured = {1.0 - 0.5 * 0.25 + 0.1 * 0.0625, 1.0 - 0.5 * 0.5 + 0.1 * 0.25, 1.0 - 0.5 * 0.75 + 0.1 * 0.5625};
        data->positions = {0.25, 0.5, 0.75};

        // パターンB: 複数種類ある場合は data_sets に積み、selectAndConcatDataSets で連結する
        // optimizer::ProductDataSet ds1;
        // ds1.data_type_id = "thickness";
        // ds1.measured = { ... }; ds1.positions = { ... };  // position は 0~1
        // data->data_sets.push_back(ds1);
        // ...
        // selectAndConcatDataSets(data.get());

        selectAndConcatDataSets(data.get());
        filterByPositionRange(data.get());

        // 結果 CSV に出す追加列（コイル番号・厚み等）
        // data->extra_columns.push_back({"coil_no", meta.product_id});
        // data->extra_columns.push_back({"thickness", "1.2"});
        return data;
    }

private:
    /** cfg の optimization_data_types に従い、使用する種類だけを選んで measured/positions に連結する。data_sets が空のときは何もしない。 */
    static void selectAndConcatDataSets(optimizer::ProductLoadedData* data) {
        if (!data || data->data_sets.empty()) return;
        data->measured.clear();
        data->positions.clear();
        for (const auto& ds : data->data_sets) {
            if (optimizer::ParaConfig::isDataTypeUsedForOptimization(ds.data_type_id)) {
                for (double v : ds.measured) data->measured.push_back(v);
                for (double p : ds.positions) data->positions.push_back(p);
            }
        }
    }

    /** cfg の optimization_position_min/max の範囲外の点を除く（範囲外は最適化対象にしない）。position は 0~1 想定。 */
    static void filterByPositionRange(optimizer::ProductLoadedData* data) {
        if (!data || data->positions.size() != data->measured.size()) return;
        const double lo = optimizer::ParaConfig::getOptimizationPositionMin();
        const double hi = optimizer::ParaConfig::getOptimizationPositionMax();
        std::vector<double> m, p;
        for (size_t i = 0; i < data->positions.size(); ++i) {
            if (data->positions[i] >= lo && data->positions[i] <= hi) {
                m.push_back(data->measured[i]);
                p.push_back(data->positions[i]);
            }
        }
        data->measured = std::move(m);
        data->positions = std::move(p);
    }
};

// =============================================================================
// 5. PLOG / LLOG / DLOG — ユーザーが作る関数（ここを実装すると CSV が出る）
// =============================================================================
// ro.PLOG_add(カラム名, 値, 書式), ro.PLOG_endRow() で 1 行。書式は空で自動。
// ro.LLOG_add(カラム名, 値, 書式), ro.LLOG_endRow() で 1 行。PLOG/LLOG は終了時またはサイズ超過で flush。
// ro.DLOG_beginProduct(product_id), ro.DLOG_add(カラム名, 値, 書式), ro.DLOG_endRow()。DLOG は endRow 毎に flush。
// ON/OFF はコンフィグ plog_enabled, llog_enabled, dlog_enabled（para.cfg）。
//
/** PLOG を出すか。コンフィグ plog_enabled を読む。ユーザーはここを差し替えてよい。 */
static bool userPLOGEnabled() { return optimizer::ParaConfig::getPLOGEnabled(); }
/** PLOG の出力先ファイル名フォーマット。{timestamp}, {product_id} 利用可。 */
static std::string userPLOGFilename() {
    std::string s = optimizer::ParaConfig::getPLOGFilename();
    return s.empty() ? "result/plog_{timestamp}.csv" : s;
}

/** LLOG を出すか。コンフィグ llog_enabled を読む。 */
static bool userLLOGEnabled() { return optimizer::ParaConfig::getLLOGEnabled(); }
/** LLOG の出力先。全製品 1 ファイル。 */
static std::string userLLOGFilename() { return optimizer::ParaConfig::getLLOGFilename(); }
/** LLOG の範囲（全製品共通）。start_index, max_points。 */
static void userLLOGRange(size_t& startIndex, size_t& maxPoints) {
    int s = optimizer::ParaConfig::getDetailStartIndex();
    int m = optimizer::ParaConfig::getDetailMaxPoints();
    startIndex = s >= 0 ? static_cast<size_t>(s) : 0u;
    maxPoints = m > 0 ? static_cast<size_t>(m) : 256u;
}

/** DLOG を出すか。コンフィグ dlog_enabled を読む。 */
static bool userDLOGEnabled() { return optimizer::ParaConfig::getDLOGEnabled(); }
/** DLOG の出力先。1 製品 1 ファイル。{product_id}, {timestamp} 利用可。 */
static std::string userDLOGFilename() { return optimizer::ParaConfig::getDLOGFilename(); }
/** DLOG の製品ごとの (start_index, max_points)。results.size() と一致させる。 */
static std::vector<std::pair<size_t, size_t>> userDlogRanges(const std::vector<optimizer::ProductRunResult>& results) {
    std::vector<std::pair<size_t, size_t>> ranges;
    ranges.reserve(results.size());
    size_t gStart, gMax;
    userLLOGRange(gStart, gMax);
    for (const auto& r : results) {
        size_t n = r.measured.size();
        size_t start = (gStart < n) ? gStart : 0u;
        size_t remain = (start < n) ? (n - start) : 0u;
        size_t maxPoints = (gMax > 0 && remain > gMax) ? gMax : remain;
        ranges.push_back({start, maxPoints});
    }
    return ranges;
}

// =============================================================================
// 5.2 OnsiteResultWriter — IResultWriter（PLOG/LLOG/DLOG は add / endRow で書き込む）
// =============================================================================
class OnsiteResultWriter : public optimizer::IResultWriter {
public:
    OnsiteResultWriter() {
        ro_.setMaxFileBytes(optimizer::ParaConfig::getResultFileMaxBytes());
        ro_.setMaxTotalBytes(optimizer::ParaConfig::getResultTotalMaxBytes());
        if (userPLOGEnabled() && !userPLOGFilename().empty()) ro_.setPLOGFilename(userPLOGFilename());
        if (userLLOGEnabled() && !userLLOGFilename().empty()) ro_.setLLOGFilename(userLLOGFilename());
        if (userDLOGEnabled() && !userDLOGFilename().empty()) ro_.setDLOGFilename(userDLOGFilename());
    }

    ~OnsiteResultWriter() {
        ro_.flushPLOG();
        ro_.flushLLOG();
    }

    void writeApplyOnly(const std::vector<double>& fullParams,
                        const std::vector<optimizer::ProductRunResult>& results) override {
        (void)fullParams;
        if (userPLOGEnabled())
            ro_.writePLOG(results, "rmse_apply");  // 列を横に追加。PLOG_add/PLOG_endRow で自前の列も可。
    }

    void writeAfterOptimization(const std::vector<double>& fullParams,
                                const std::vector<optimizer::ProductRunResult>& results) override {
        (void)fullParams;
        if (userPLOGEnabled())
            ro_.writePLOG(results, "rmse_after");

        if (userLLOGEnabled() && !userLLOGFilename().empty()) {
            size_t startIndex, maxPoints;
            userLLOGRange(startIndex, maxPoints);
            for (const auto& r : results) {
                if (!r.ok) continue;
                size_t n = r.measured.size();
                for (size_t k = 0; k < maxPoints && startIndex + k < n; ++k) {
                    size_t idx = startIndex + k;
                    ro_.LLOG_add("product_id", r.product_id, "");
                    ro_.LLOG_add("point_index", static_cast<int>(k), "");
                    ro_.LLOG_add("measured", r.measured[idx], "");
                    ro_.LLOG_add("predicted", r.predicted[idx], "");
                    ro_.LLOG_add("residual", r.residuals[idx], "");
                    for (const auto& p : r.extra_columns)
                        ro_.LLOG_add(p.first, p.second, "");
                    ro_.LLOG_endRow();
                }
            }
        }

        if (userDLOGEnabled() && !userDLOGFilename().empty()) {
            std::vector<std::pair<size_t, size_t>> ranges = userDlogRanges(results);
            if (ranges.size() != results.size()) return;
            for (size_t ri = 0; ri < results.size(); ++ri) {
                const auto& r = results[ri];
                if (!r.ok) continue;
                ro_.DLOG_beginProduct(r.product_id);
                size_t startIndex = ranges[ri].first;
                size_t maxPoints = ranges[ri].second;
                size_t n = r.measured.size();
                for (size_t k = 0; k < maxPoints && startIndex + k < n; ++k) {
                    size_t idx = startIndex + k;
                    ro_.DLOG_add("product_id", r.product_id, "");
                    ro_.DLOG_add("point_index", static_cast<int>(k), "");
                    ro_.DLOG_add("measured", r.measured[idx], "");
                    ro_.DLOG_add("predicted", r.predicted[idx], "");
                    ro_.DLOG_add("residual", r.residuals[idx], "");
                    for (const auto& p : r.extra_columns)
                        ro_.DLOG_add(p.first, p.second, "");
                    ro_.DLOG_endRow();
                }
            }
        }
    }

private:
    optimizer::RO ro_;
};

// =============================================================================
// 6. 汎用 CSV 書き出し例（RO = ResultOutput の 2 タイミング同一ファイル追記）
// =============================================================================
// PLOG/LLOG/DLOG は「5. ユーザーが作る関数」の userPLOGFilename / userLLOGConfig / userDLOGFilename / userDlogRanges を実装すると出る。
// 以下はそれとは別の、汎用の addColumn/endRow/flush の例。
//
// 実行すると result/example_onsite_{timestamp}.csv にサンプルが出力される。
// PLOG と同様に、setFilenameSame → 行追加 → flush(Before) → 行追加 → flush(After) で同一ファイルに追記。
/** RO を使った CSV 書き出しの最小例 */
static void writeExampleCsv() {
    ensureResultDir();
    optimizer::RO ro;
    ro.setFilenameSame("result/example_onsite_{timestamp}.csv");

    // 1 行目: ヘッダは addColumn の初出で決まる。列を追加して endRow で行確定。
    ro.addColumn("product_id", "sample_A");
    ro.addColumn("rmse", 0.0123);
    ro.addColumn("n_points", 10);
    ro.endRow();
    ro.addColumn("product_id", "sample_B");
    ro.addColumn("rmse", 0.0456);
    ro.addColumn("n_points", 20);
    ro.endRow();
    ro.flush(optimizer::RO::Before);  // ヘッダ + 上記 2 行を書き出し

    // 同じファイルに「後」の行だけ追記（ヘッダは出さない）
    ro.addColumn("product_id", "sample_A");
    ro.addColumn("rmse", 0.0089);
    ro.addColumn("n_points", 10);
    ro.endRow();
    ro.addColumn("product_id", "sample_B");
    ro.addColumn("rmse", 0.0321);
    ro.addColumn("n_points", 20);
    ro.endRow();
    ro.flush(optimizer::RO::After);   // 上記 2 行を追記

    // 別ファイルに 1 本だけ書きたい場合は setFilename で前後を分ける
    // ro.clear();
    // ro.setFilename(optimizer::RO::Before, "result/only_before_{timestamp}.csv");
    // ro.setFilename(optimizer::RO::After,  "result/only_after_{timestamp}.csv");
    // ro.addColumn("key", "val"); ro.endRow(); ro.flush(optimizer::RO::Before);
}

}  // namespace

int main() {
    const std::string configPath = "config/para.cfg";

    Handler handler(configPath);
    optimizer::DataConfig::load(configPath);
    if (!optimizer::ParaConfig::isOptimizerListValid()) {
        optimizer::TerminalMessage::error(optimizer::ParaConfig::getOptimizerListError());
        return 1;
    }

    if (optimizer::ParaConfig::isTraceEnabled() || optimizer::ParaConfig::isDebugEnabled())
        ensureLogDir();
    static std::ofstream s_debugLog;
    if (optimizer::ParaConfig::isDebugEnabled()) {
        if (optimizer::openLogWithRotation("log/debug.log", s_debugLog, optimizer::ParaConfig::getDebugLogMaxBytes()))
            optimizer::ParaConfig::setDebugStream(&s_debugLog);
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

    // 結果をファイル/DB へ出す場合は OnsiteResultWriter を渡す。
    // サンプル: 前・後を同じ CSV に出す場合 — 先に runApplyOnly で「前」を書き、run で「後」を追記する。
    OnsiteResultWriter resultWriter;
    optimizer::OptimizerDriver::runApplyOnly(mapper, model, loader, products, nullptr, resultWriter);

    const std::string optimizerName = handler.getOptimizersToRun()[0];
    std::string tracePath = optimizer::ParaConfig::isTraceEnabled() ? "log/onsite_trace.csv" : "";
    optimizer::RunResult result = optimizer::OptimizerDriver::run(
        configPath, mapper, model, loader, products, optimizerName,
        tracePath, "onsite", &resultWriter);

    std::cout << "[Onsite] optimizer=" << optimizerName
              << " bestScore=" << result.bestScore
              << " bestParams=";
    for (size_t i = 0; i < result.bestParams.size(); ++i)
        std::cout << (i ? "," : "") << result.bestParams[i];
    std::cout << "\n";

    // CSV 書き出し例: result/example_onsite_{timestamp}.csv にサンプルを出力
    writeExampleCsv();

    return 0;
}
