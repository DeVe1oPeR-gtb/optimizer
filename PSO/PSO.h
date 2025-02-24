#ifndef D_PSO_H
#define D_PSO_H

#include "Optimizer.h"
#include <cmath>
#include <limits>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

template <typename T>
class PSO : public Optimizer {
  public:
    struct Particle {
        std::vector<T> position;
        std::vector<T> velocity;
        std::vector<T> best_position;
        T my_score;
        T best_score;
        std::vector<std::pair<T, T>> eval_data;
        typename Optimizer::Stats<T> my_stats;
        typename Optimizer::Stats<T> best_stats;
        int no;
    };

    struct Gbest {
        int num_particle;
        std::vector<T> position;
        T score;
        typename Optimizer::Stats<T> stats;
    };

    explicit PSO(int, int, std::vector<T>, std::vector<T>, std::vector<T>, std::vector<T>, std::vector<T>);
    virtual ~PSO();

    const std::vector<Particle> &getParticles();
    void setParticles(const std::vector<std::vector<T>> &, const std::vector<std::vector<T>> &);
    void initParticles(const std::vector<T> &, const std::vector<T> &);
    void setEvalData(const int &, const std::vector<std::pair<T, T>> &);
    typename Optimizer::Stats<T> calcPersonalScore(const int &);
    void updatePersonalBest(const int &, const T &);
    void updateGlobalBest();
    const Gbest &getGlobalBest();
    void updateParticles();

  private:
    std::vector<Particle> particles_;
    Gbest gbest_;
    int num_dim_;
    std::vector<T> w_;  // 慣性
    std::vector<T> c1_; // Pbest重み
    std::vector<T> c2_; // Gbest重み

    std::mt19937 rng_;
    std::vector<T> ul_bound_;
    std::vector<T> ll_bound_;
    void printParticles(int);
};

#endif // D_PSO_H