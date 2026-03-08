#ifndef OPTIMIZER_UTIL_PARA_CONFIG_H
#define OPTIMIZER_UTIL_PARA_CONFIG_H

/**
 * @file ParaConfig.hpp
 * @brief 最適化まわりの設定（trace, optimizer, 各最適化器のパラメータ）の読込と取得。
 */

#include "util/util_common.hpp"
#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

namespace optimizer {

/**
 * @brief 最適化まわりの設定読込（config/para.cfg）
 *
 * trace, optimizer, lm_apply_bounds, n_iter_*, pso_*, de_*, lm_* を解釈。
 * 1行に複数指定する場合は key=val を ; で区切る（試行錯誤用にコピペしやすい）。
 */
class ParaConfig {
public:
    /** @brief 設定ファイルを読み、全パラメータを更新 */
    static void load(const std::string& path = "config/para.cfg");

    /** @brief RunConfig の内容で設定を上書き（ファイルを読まない） */
    static void loadFromStruct(const RunConfig& config);

    /** @brief トレース出力が有効か */
    static bool isTraceEnabled();

    /** @brief デバッグログが有効か（cfg の debug=on で true）。trace とは別。 */
    static bool isDebugEnabled();

    /** @brief デバッグログ出力先（nullptr のときは stderr に出力）。caller が setDebugStream で設定可能。 */
    static std::ostream* getDebugStream();
    static void setDebugStream(std::ostream* s);

    /** @brief デバッグが有効なときのみメッセージをログ出力（改行付き）。出力先は getDebugStream()、未設定時は stderr。 */
    static void logDebug(const std::string& message);

    /** @brief トレースログのローテート閾値（バイト）。このサイズ以上で .bak に退避してから新規に開く。0 でローテートなし。 */
    static size_t getTraceLogMaxBytes();
    /** @brief デバッグログのローテート閾値（バイト）。同上。 */
    static size_t getDebugLogMaxBytes();

    /** @brief LM でパラメータ上下限を適用するか（para.cfg の lm_apply_bounds） */
    static bool isLmApplyBoundsEnabled();

    /** @brief 実行する最適化器名のリスト。PSO, DE, LM のいずれかまたは ALL_OPT_EXEC（隠し）で全実行。未指定・不正時はエラー。 */
    static const std::vector<std::string>& getOptimizersToRun();
    /** @brief optimizer= が有効に指定されているか。false のとき getOptimizerListError() で理由を取得。 */
    static bool isOptimizerListValid();
    /** @brief 最適化器リストが無効なときのエラー文言（空でなければエラー）。 */
    static const std::string& getOptimizerListError();

    /** @brief トレース出力先（デフォルトは nullptr = 無効）。caller が setTraceStream で設定。 */
    static std::ostream* getTraceStream();
    static void setTraceStream(std::ostream* s);

    /** @brief 最適化器の反復数 */
    static int getNIterPso();
    static int getNIterDe();
    static int getNIterLm();

    /** @brief PSO: 慣性 w, パーソナルベスト重み c1, グローバルベスト重み c2, 粒子数, 初期範囲の半径 */
    static double getPsoW();
    static double getPsoC1();
    static double getPsoC2();
    static int getPsoNParticle();
    static double getPsoInitRadius();

    /** @brief DE: 変異係数 F, 交叉率 CR, 個体数, 初期範囲の半径 */
    static double getDeF();
    static double getDeCr();
    static int getDeNPop();
    static double getDeInitRadius();

    /** @brief LM: 初期λ, 摂動 r, λの最小/最大, 採用時縮小率/却下時拡大率, 1反復あたり最大試行回数 */
    static double getLmLambdaInit();
    static double getLmRPerturb();
    static double getLmLambdaMin();
    static double getLmLambdaMax();
    static double getLmLambdaDown();
    static double getLmLambdaUp();
    static int getLmMaxTry();

    /** @brief PLOG / LLOG / DLOG の書き出し ON/OFF。コンフィグで plog_enabled, llog_enabled, dlog_enabled */
    static bool getPLOGEnabled();
    static bool getLLOGEnabled();
    static bool getDLOGEnabled();
    /** @brief PLOG 出力先ファイル名フォーマット（空なら無効）。{timestamp}, {product_id} 等利用可 */
    static const std::string& getPLOGFilename();
    /** @brief 汎用 CSV 最適化後ファイル名（2 タイミング書き出しの後用）。空なら無効 */
    static const std::string& getCsvFilenameAfter();
    /** @brief LLOG/DLOG 有効フラグ, 開始インデックス, 最大点数, LLOG を 1 ファイルにまとめるか */
    static bool getDetailEnabled();
    static int getDetailStartIndex();
    static int getDetailMaxPoints();
    static bool getLLOGOneFile();
    /** @brief LLOG ファイル名フォーマット（全製品 1 ファイル） */
    static const std::string& getLLOGFilename();
    /** @brief DLOG ファイル名フォーマット（1 製品 1 ファイル、{product_id} 利用可） */
    static const std::string& getDLOGFilename();
    /** @brief 1ファイル最大バイト, 合計最大バイト（0=制限なし）。超えたら警告して書き出しスキップ */
    static size_t getResultFileMaxBytes();
    static size_t getResultTotalMaxBytes();
    /** @brief 最適化終了パラメータを書き出すテキストファイルパス（空なら無効） */
    static const std::string& getResultFinalParamsFilename();

    /** @brief 実績・予測の種類ごとの最適化使用フラグ（cfg の optimization_data_types=A,B で指定。空なら全種類を使用） */
    static const std::vector<std::string>& getOptimizationDataTypes();
    /** @brief 指定したデータ種類を最適化に使うか。リストが空のときは true（全種類使用） */
    static bool isDataTypeUsedForOptimization(const std::string& data_type_id);

    /** @brief position の有効範囲（0~1）。範囲外の点は最適化対象から外す。cfg の optimization_position_min/max（例: 0.05, 0.95） */
    static double getOptimizationPositionMin();
    static double getOptimizationPositionMax();

    /** @brief テスト用: load() で設定した静的文字列・vector をクリアし、リーク検出前にメモリを解放する */
    static void resetForTest();

private:
    static bool traceEnabled_;
    static bool debugEnabled_;
    static std::ostream* traceStream_;
    static std::ostream* debugStream_;
    static std::vector<std::string> optimizersToRun_;
    static bool optimizerListValid_;
    static std::string optimizerListError_;
    static bool lmApplyBoundsEnabled_;
    static int nIterPso_;
    static int nIterDe_;
    static int nIterLm_;
    static double psoW_;
    static double psoC1_;
    static double psoC2_;
    static int psoNParticle_;
    static double psoInitRadius_;
    static double deF_;
    static double deCr_;
    static int deNPop_;
    static double deInitRadius_;
    static double lmLambdaInit_;
    static double lmRPerturb_;
    static double lmLambdaMin_;
    static double lmLambdaMax_;
    static double lmLambdaDown_;
    static double lmLambdaUp_;
    static int lmMaxTry_;
    static size_t traceLogMaxBytes_;
    static size_t debugLogMaxBytes_;
    static bool plogEnabled_;
    static bool llogEnabled_;
    static bool dlogEnabled_;
    static std::string plogFilename_;
    static std::string csvFilenameAfter_;
    static bool detailEnabled_;
    static int detailStartIndex_;
    static int detailMaxPoints_;
    static bool llogOneFile_;
    static std::string llogFilename_;
    static std::string dlogFilename_;
    static size_t resultFileMaxBytes_;
    static size_t resultTotalMaxBytes_;
    static std::string resultFinalParamsFilename_;
    static std::vector<std::string> optimizationDataTypes_;
    static double optimizationPositionMin_;
    static double optimizationPositionMax_;
};

}  // namespace optimizer

#endif  // OPTIMIZER_UTIL_PARA_CONFIG_H
