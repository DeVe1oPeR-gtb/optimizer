#ifndef OPTIMIZER_UTIL_TRACE_CONFIG_H
#define OPTIMIZER_UTIL_TRACE_CONFIG_H

/**
 * @file TraceConfig.h
 * @brief 最適化まわりの設定（trace, optimizer, 各最適化器のパラメータ）の読込と取得。
 */

#include "util/util_common.hpp"
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
class TraceConfig {
public:
    /** @brief 設定ファイルを読み、全パラメータを更新 */
    static void load(const std::string& path = "config/para.cfg");

    /** @brief RunConfig の内容で設定を上書き（ファイルを読まない） */
    static void loadFromStruct(const RunConfig& config);

    /** @brief トレース出力が有効か */
    static bool isTraceEnabled();

    /** @brief LM でパラメータ上下限を適用するか（para.cfg の lm_apply_bounds） */
    static bool isLmApplyBoundsEnabled();

    /** @brief 実行する最適化器名のリスト（未指定時は PSO, DE, LM） */
    static const std::vector<std::string>& getOptimizersToRun();

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

private:
    static bool traceEnabled_;
    static std::ostream* traceStream_;
    static std::vector<std::string> optimizersToRun_;
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
};

}  // namespace optimizer

#endif  // OPTIMIZER_UTIL_TRACE_CONFIG_H
