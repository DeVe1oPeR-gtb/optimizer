#ifndef OPTIMIZER_PARAM_PARAMETER_MAPPER_H
#define OPTIMIZER_PARAM_PARAMETER_MAPPER_H

#include "ParamSpec.h"
#include <vector>
#include <string>

namespace optimizer {

/**
 * @brief db_key から初期値を返す関数の型
 *
 * USERWORK: 現場DBファイルから db_key に対応する値を取得する実装に差し替える。
 * 本番では実 DB から読み、未指定時は init_value を用いる想定。
 */
using DbValueProvider = double (*)(const std::string& db_key);

/**
 * @brief 全パラメータ集合と最適化ベクトルの対応付け
 *
 * 物理モデル用の全パラメータ（CSV 行順）と、enable_opt された部分だけのベクトルを相互変換する。
 */
class ParameterMapper {
public:
    ParameterMapper() = default;

    /**
     * @brief 検証済み設定から構築（specs の並び = 全パラメータの並び）
     */
    void setSpecs(std::vector<ParamSpec> specs);

    /**
     * @brief 検証で許容するパラメータ名を設定（空なら任意）
     */
    void setAllowedParamNames(std::vector<std::string> names);

    /**
     * @brief 現在の specs を検証
     * @param[out] errorMessage 不正時にメッセージを格納
     * @return 有効なら true
     */
    bool validate(std::string& errorMessage) const;

    /** @brief 最適化ベクトルの次元数 */
    size_t numOptParams() const;
    /** @brief 全パラメータの数 */
    size_t numFullParams() const;

    /**
     * @brief 初期ベクトル（init_mode: manual / db / hybrid に従う）
     * @param dbValueProvider db/hybrid 時に使用。nullptr のときは init_value を使用
     */
    std::vector<double> getInitialVector(DbValueProvider dbValueProvider = nullptr) const;

    /** @brief 最適化ベクトル用の下限（getInitialVector と同じ順） */
    std::vector<double> getLowerBounds() const;
    /** @brief 最適化ベクトル用の上限 */
    std::vector<double> getUpperBounds() const;

    /**
     * @brief 最適化ベクトルを全パラメータに展開（specs の並び）
     * USERWORK: どのインデックスがどの物理パラメータかは製品仕様に依存する。
     */
    std::vector<double> expandToFullParameterSet(const std::vector<double>& x_opt) const;

    /** @brief 全パラメータベクトルから最適化ベクトルを抽出 */
    std::vector<double> fullToOptVector(const std::vector<double>& full) const;

    /** @brief 現在の設定リスト */
    const std::vector<ParamSpec>& specs() const { return specs_; }

private:
    std::vector<ParamSpec> specs_;
    std::vector<std::string> allowedParamNames_;
    std::vector<size_t> optIndices_;  /**< 各最適化次元が full の何番目か */
};

}  // namespace optimizer

#endif  // OPTIMIZER_PARAM_PARAMETER_MAPPER_H
