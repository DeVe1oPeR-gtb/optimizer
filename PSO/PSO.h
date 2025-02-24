#ifndef D_PSO_H
#define D_PSO_H

#include "Optimizer.h"
#include <cmath>
#include <limits>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

class PSO : public Optimizer {
  public:
    struct Particle {
        std::vector<double> position;
        std::vector<double> velocity;
        std::vector<double> best_position;
        double my_score;
        double best_score;
        std::vector<std::pair<double, double>> eval_data;
        Optimizer::Stats my_stats;
        Optimizer::Stats best_stats;
        int no;
    };

    struct Gbest {
        int num_particle;
        std::vector<double> position;
        double score;
        Optimizer::Stats stats;
    };

    explicit PSO(int, int, std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>,
                 std::vector<double>
                 // std::vector<double>, std::vector<double>
    );
    virtual ~PSO();

    const std::vector<Particle> &getParticles();
    void setParticles(const std::vector<std::vector<double>> &, const std::vector<std::vector<double>> &);
    void initParticles(const std::vector<double> &, const std::vector<double> &);
    void setEvalData(const int &, const std::vector<std::pair<double, double>> &);
    Optimizer::Stats calcPersonalScore(const int &);
    void updatePersonalBest(const int &, const double &);
    void updateGlobalBest();
    const Gbest &getGlobalBest();
    void updateParticles();

  private:
    std::vector<Particle> particles_;
    Gbest gbest_;
    int num_dim_;
    std::vector<double> w_;  // 慣性
    std::vector<double> c1_; // Pbest重み
    std::vector<double> c2_; // Gbest重み

    std::mt19937 rng_;
    std::vector<double> ul_bound_;
    std::vector<double> ll_bound_;
    // std::vector<double> ul_ini_bound_;
    // std::vector<double> ll_ini_bound_;
    void printParticles(int);
};

#endif