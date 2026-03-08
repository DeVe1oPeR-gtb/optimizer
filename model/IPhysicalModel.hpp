#ifndef OPTIMIZER_MODEL_I_PHYSICAL_MODEL_H
#define OPTIMIZER_MODEL_I_PHYSICAL_MODEL_H

#include <vector>

namespace optimizer {

/**
 * @brief 1製品用物理モデルのインターフェース
 *
 * USERWORK: 入出力の型・並びは製品仕様に依存する。ここでは汎用の vector を使用。
 * 1製品につき1回呼び出し、実測/位置と同じ長さの予測ベクトルを返す。
 */
class IPhysicalModel {
public:
    virtual ~IPhysicalModel() = default;

    /**
     * @brief 1製品分のモデル実行
     * @param fullParams その製品用の全パラメータ
     * @param productLoadedData ロード済み製品データ（実測・位置等）
     * @return 実測と同じ位置での予測値（1:1 対応）
     *
     * USERWORK: 実物理モデルに差し替え。入出力レイアウトは製品仕様に合わせる。
     */
    virtual std::vector<double> run(const std::vector<double>& fullParams,
                                   const void* productLoadedData) = 0;
};

}  // namespace optimizer

#endif  // OPTIMIZER_MODEL_I_PHYSICAL_MODEL_H
