#ifndef OPTIMIZER_CORE_HPP
#define OPTIMIZER_CORE_HPP

/**
 * @file core.hpp
 * @brief 目的関数評価の型とインターフェース（EvalResult, JacobianResult, IObjective, IDifferentiableObjective）
 */

#include <vector>

namespace optimizer {

// --- EvalResult ---
/**
 * @brief 1回の目的関数評価の結果
 *
 * objective は残差のノルム（例: sqrt(sum(r^2))）など。residuals は全製品の残差を連結したベクトル。
 */
struct EvalResult {
    double objective = 0.0;           /**< 目的関数値（スカラ） */
    std::vector<double> residuals;   /**< 残差ベクトル（全製品連結） */
};

// --- JacobianResult ---
/**
 * @brief ヤコビアン付き評価の結果（LM 等の勾配法用）
 *
 * jacobian[i][j] = d(residual_i) / d(x_j)。行優先で [残差数][パラメータ数]。
 */
struct JacobianResult {
    std::vector<double> residuals;   /**< 残差ベクトル */
    /** ヤコビ行列 [残差数][パラメータ数]、行優先 */
    std::vector<std::vector<double>> jacobian;
};

// --- IObjective ---
/**
 * @brief 目的関数評価の共通インターフェース
 *
 * 最適化器（PSO/DE/LM）はこの evaluate(x) のみを呼ぶ。戻り値の残差ベクトルは
 * 全製品の残差を連結したもので、目的スカラ（例: RMSE）はそのノルムから導出する。
 */
class IObjective {
public:
    virtual ~IObjective() = default;

    /**
     * @brief 最適化ベクトル x での評価（enable_opt されたパラメータのみ）
     * @param x 最適化対象パラメータベクトル
     * @return 目的値と残差ベクトル（全製品連結）
     */
    virtual EvalResult evaluate(const std::vector<double>& x) = 0;
};

// --- IDifferentiableObjective ---
/**
 * @brief 残差とヤコビアンを返せる目的関数（LM 用）
 *
 * evaluateWithJacobian(x) で残差ベクトルと jacobian[残差添字][パラメータ添字] を返す。
 * 本ライブラリでは Objective が数値微分で実装している。
 */
class IDifferentiableObjective : public IObjective {
public:
    ~IDifferentiableObjective() override = default;

    /**
     * @brief x での評価とヤコビアン（例: 数値微分）
     * @param x 最適化対象パラメータベクトル
     * @return 残差とヤコビ行列
     */
    virtual JacobianResult evaluateWithJacobian(const std::vector<double>& x) = 0;
};

}  // namespace optimizer

#endif  // OPTIMIZER_CORE_HPP
