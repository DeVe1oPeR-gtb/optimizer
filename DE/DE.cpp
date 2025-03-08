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

    mutants_.resize(pop_size);
    for (auto &m : mutants_) {
        m.position.resize(num_dim_);
        for (int d = 0; d < num_dim_; ++d) {
            std::uniform_real_distribution<T> dist(ll_bound_[d], ul_bound_[d]);
            m.position[d] = dist(rng_);
        }
        m.score = std::numeric_limits<T>::max();
        m.stats.initialize();
    }
}

template <typename T>
DE<T>::~DE() = default;

template <typename T>
const std::vector<typename DE<T>::Individual> &DE<T>::getPopulation() {
    return population_;
}

template <typename T>
const std::vector<typename DE<T>::Individual> &DE<T>::getMutants() {
    return mutants_;
}

template <typename T>
void DE<T>::initPopulation(const std::vector<T> &ul_ini_bound, const std::vector<T> &ll_ini_bound) {
    auto m = mutants_.begin();
    for (auto &ind : population_) {
        for (int d = 0; d < num_dim_; ++d) {
            std::uniform_real_distribution<T> dist(ll_ini_bound[d], ul_ini_bound[d]);
            ind.position[d] = dist(rng_);
            m->position[d] = ind.position[d];
        }
        m++;
    }
}

template <typename T>
void DE<T>::setEvalData(const int &n, const std::vector<std::pair<T, T>> &eval_data) {
    mutants_[n].stats = Optimizer::computeStats(eval_data);
}

template <typename T>
typename Optimizer::Stats<T> DE<T>::calcScore(const int &n) {
    mutants_[n].score = mutants_[n].stats.rmse;
    return mutants_[n].stats;
}

template <typename T>
void DE<T>::mutation() {
    std::uniform_real_distribution<T> dist(0.0, 1.0);
    std::uniform_int_distribution<int> idx_dist(0, population_.size() - 1);

    auto m = mutants_.begin();

    for (auto &ind : population_) {
        int a, b, c;
        do { a = idx_dist(rng_); } while (a == &ind - &population_[0]);
        do { b = idx_dist(rng_); } while (b == a || b == &ind - &population_[0]);
        do { c = idx_dist(rng_); } while (c == b || c == a || c == &ind - &population_[0]);

        int R = idx_dist(rng_);
        for (int d = 0; d < num_dim_; ++d) {
            if (dist(rng_) < CR_ || d == R) {
                m->position[d] = population_[a].position[d] + F_ * (population_[b].position[d] - population_[c].position[d]);
                m->position[d] = std::clamp(m->position[d], ll_bound_[d], ul_bound_[d]);
            }
            else{
                m->position[d] = ind.position[d];
            }
        }
        m++;
    }
}

template <typename T>
void DE<T>::selection() {
    auto m = mutants_.begin();
    for (auto &ind : population_) {
        // std::cout << "Comparing scores: mutant " << m->score << " vs individual " << ind.score << std::endl;

        if (m->score < ind.score) {
            ind = *m;
            // std::cout << "Individual " << &ind - &population_[0] << " updated." << std::endl;
        }
        m++;
    }
}

// 明示的インスタンス化
template class DE<double>;
template class DE<float>;
