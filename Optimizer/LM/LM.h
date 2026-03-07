#ifndef D_LM_H
#define D_LM_H

#include "Optimizer.h"
#include <cmath>
#include <limits>
#include <numeric>
#include <random>
#include <utility>
#include <vector>
#include <iosfwd>


// Levenberg-Marquardt method 
template <typename T>
class LM : public Optimizer {
  public:
    explicit LM(int,int,const std::vector<T>&);
    virtual ~LM();

    void setJacobian(const int&, const std::vector<T>&);
    void setEvalData(const std::vector<std::pair<T,T>>&);

    const std::vector<std::vector<T>>& getJacobian();
    const std::vector<T>& getDelta();
    const std::vector<T>& getNextDelta();
    const Optimizer::Stats<T>& getStats();

    /** トレース出力（開発者用設定でON時のみ有効）。z は現在のパラメータベクトル。 */
    void setTraceStream(std::ostream* s) { traceStream_ = s; }
    void setTraceEnabled(bool en) { traceEnabled_ = en; }
    void writeTraceLine(int iteration, const std::vector<T>& z) const;

  private:
    // 実行時定数
    int num_dim_;
    int num_data_;
    T lambda_;             // ダンピングパラメータ
    std::vector<T> r_;     // 摂動量
  
    // ワーク
    std::vector<std::vector<T>> J_;         // ヤコビアン[データ数][パラメータ数]
    std::vector<std::vector<T>> Jt_;        // 転置ヤコビアン[データ数][パラメータ数]
    std::vector<std::vector<T>> Ad_;        // 拡大係数行列
                                            //   :近似ヘッセ行列 + 次回ステップサイズdelta
    std::vector<T> delta_;                  // ステップサイズ
    std::vector<std::pair<T,T>> eval_data_; // 評価データ
    Optimizer::Stats<T> stats_;
    std::ostream* traceStream_ = nullptr;
    bool traceEnabled_ = false;
};
#endif