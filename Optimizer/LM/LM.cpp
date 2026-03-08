#include "LM/LM.hpp"

// helper
template <typename T>
void gjm(const int&, std::vector<std::vector<T>>&);


template <typename T>
LM<T>::LM(int num_dim_, int num_data_, const std::vector<T>& z, T r_perturb)
    : num_dim_(num_dim_), num_data_(num_data_)
{
    J_.resize(num_data_, std::vector<T>(num_dim_, 0.0));
    Jt_.resize(num_data_, std::vector<T>(num_dim_, 0.0));
    Ad_.resize(num_dim_, std::vector<T>(num_dim_ + 1, 0.0));
    delta_.resize(num_dim_, 0.0);

    lambda_ = 10.0;  // 初期値は呼び出し側で setLambda により上書き可能
    r_.resize(num_dim_, r_perturb);

    for (int d = 0; d < num_dim_; ++d) delta_[d] = z[d] * r_[d];
}

template <typename T> 
LM<T>::~LM() = default;

template <typename T>
void LM<T>::setJacobian(const int& idx_param, const std::vector<T>& dfdz){
    // ヤコビ行列をセットする
    for (int i=0; i < num_data_; ++i){
        J_[i][idx_param] = dfdz[i];
        Jt_[i][idx_param] = dfdz[i];
    }
    return;
}

template <typename T>
void LM<T>::setEvalData(const std::vector<std::pair<T,T>>& eval_data){
    eval_data_ = eval_data;
    return;
}

template <typename T>
const std::vector<std::vector<T>>& LM<T>::getJacobian(){
    return J_;
}

template <typename T>
const std::vector<T>& LM<T>::getDelta(){
    return delta_;
}

//  Levenberg-Marquardt method（レーヴェンバーグ・マルカート法）
template <typename T>
const std::vector<T>& LM<T>::getNextDelta(){
    // 初期化
    for (int d1 = 0; d1 < num_dim_; ++d1) {
        for (int d2 = 0; d2 < num_dim_ + 1; ++d2) {
            Ad_[d1][d2] = 0.0;
        }
    }
    // 近似ヘッセ行列を計算する
    for (int k = 0; k < num_data_; ++k) {
        for (int d1 = 0; d1 < num_dim_; ++d1) {
            for (int d2 = 0; d2 < num_dim_; ++d2) {
                Ad_[d1][d2] += Jt_[k][d1] * J_[k][d2];
            }
            Ad_[d1][num_dim_] += Jt_[k][d1] * (eval_data_[k].first - eval_data_[k].second);
        }
    }
    for (int d1 = 0; d1 < num_dim_; ++d1) Ad_[d1][d1] += lambda_; // 対角成分にダンピングパラメータを加える

    gjm(num_dim_, Ad_); // 連立1次方程式を解く
    for (int i = 0; i < num_dim_; ++i){
        delta_[i] = Ad_[i][num_dim_];
    }
    return delta_;
}

template <typename T>
const Optimizer::Stats<T>& LM<T>::getStats() {
    stats_ = Optimizer::computeStats(eval_data_);
    return stats_;
}

template <typename T>
void LM<T>::writeTraceLine(int iteration, const std::vector<T>& z) const {
    if (!traceEnabled_ || !traceStream_) return;
    Optimizer::Stats<T> s = Optimizer::computeStats(eval_data_);
    auto& out = *traceStream_;
    out << iteration << "," << s.rmse;
    for (const auto& v : z) out << "," << v;
    out << "\n";
}

template class LM<double>;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// helper
template <typename T>
void gjm(const int& n, std::vector<std::vector<T>>& A) {
    T b,c;
    for (int k = 0; k < n; ++k){
        c = A[k][k];  // [TODO] 0割チェック
        for (int j = k; j <= n; ++j){
            A[k][j] = A[k][j] / c;
        }
        for (int i = 0; i < n; ++i){
            if( i != k ){
                b = A[i][k];
                for (int j = k; j <= n; ++j){
                    A[i][j] = A[i][j] -  A[k][j] * b;
                }
            }
        }
    }
    return;
}
