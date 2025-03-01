#include "DE.h"
#include <algorithm>
#include <limits>

template <typename T>
DE<T>::DE(int pop_size, int num_dim, T F, T CR, std::vector<T> ul_bound, std::vector<T> ll_bound)
    : Optimizer(), num_dim_(num_dim), F_(F), CR_(CR), ul_bound_(ul_bound), ll_bound_(ll_bound),
      rng_(std::random_device{}()) {

    population_.resize(pop_size);
    for (auto &ind : population_) {
        ind.position.resize(num_dim_);
        for (int d = 0; d < num_dim_; ++d) {
            std::uniform_real_distribution<T> dist(ll_bound_[d], ul_bound_[d]);
            ind.position[d] = dist(rng_);
        }
        ind.score = std::numeric_limits<T>::max();
        ind.stats.initialize();
    }
}

template <typename T>
DE<T>::~DE() = default;

template <typename T>
const std::vector<typename DE<T>::Individual> &DE<T>::getPopulation() {
    return population_;
}

template <typename T>
void DE<T>::initPopulation(const std::vector<T> &ul_ini_bound, const std::vector<T> &ll_ini_bound) {
    for (auto &ind : population_) {
        for (int d = 0; d < num_dim_; ++d) {
            std::uniform_real_distribution<T> dist(ll_ini_bound[d], ul_ini_bound[d]);
            ind.position[d] = dist(rng_);
        }
    }
}

template <typename T>
void DE<T>::setEvalData(const int &n, const std::vector<std::pair<T, T>> &eval_data) {
    population_[n].stats = Optimizer::computeStats(eval_data);
}

template <typename T>
typename Optimizer::Stats<T> DE<T>::calcScore(const int &n) {
    return population_[n].stats;
}

template <typename T>
void DE<T>::updatePopulation() {
    std::uniform_real_distribution<T> dist(0.0, 1.0);
    std::uniform_int_distribution<int> idx_dist(0, population_.size() - 1);

    for (auto &ind : population_) {
        int a, b, c;
        do { a = idx_dist(rng_); } while (a == &ind - &population_[0]);
        do { b = idx_dist(rng_); } while (b == a || b == &ind - &population_[0]);
        do { c = idx_dist(rng_); } while (c == b || c == a || c == &ind - &population_[0]);

        Individual trial = ind;
        int R = idx_dist(rng_);
        for (int d = 0; d < num_dim_; ++d) {
            if (dist(rng_) < CR_ || d == R) {
                trial.position[d] = population_[a].position[d] + F_ * (population_[b].position[d] - population_[c].position[d]);
                trial.position[d] = std::clamp(trial.position[d], ll_bound_[d], ul_bound_[d]);
            }
        }

        if (trial.stats.rmse < ind.stats.rmse) {
            ind = trial;
        }
    }
}

// 明示的インスタンス化
template class DE<double>;
template class DE<float>;
