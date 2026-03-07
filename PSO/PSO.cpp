#include "PSO.h"
#include <iomanip>

template <typename T>
PSO<T>::PSO(int num_particles, int num_dim, std::vector<T> w, std::vector<T> c1, std::vector<T> c2, std::vector<T> ul_bound, std::vector<T> ll_bound)
    : Optimizer(), // 基底クラスのコンストラクタを明示的に呼ぶ
      num_dim_(num_dim), w_(w), c1_(c1), c2_(c2), ul_bound_(ul_bound), ll_bound_(ll_bound),
      rng_(std::random_device{}()) {

    // 粒子の初期化
    particles_.resize(num_particles);
    for (int i = 0; i < particles_.size(); ++i) {
        Particle *p = &particles_[i];
        p->position.resize(num_dim_);
        p->velocity.resize(num_dim_, 0.0); // 速度は0に初期化
        p->best_position.resize(num_dim_);
        for (int d = 0; d < num_dim_; ++d) {
            T ul = ul_bound_[d];
            T ll = ll_bound_[d];
            std::uniform_real_distribution<T> dist(ll, ul);
            p->position[d]      = dist(rng_);
            p->best_position[d] = p->position[d];
            p->my_stats.initialize();
            p->best_stats.initialize();
            p->no = i;
        }
        p->my_score   = std::numeric_limits<T>::max();
        p->best_score = std::numeric_limits<T>::max();
    }

    // PSO群のグローバルベスト情報の初期化
    gbest_.num_particle = 0;
    gbest_.position.resize(num_dim_);
    gbest_.position = particles_[0].position;
    gbest_.score    = std::numeric_limits<T>::max();
}

template <typename T>
PSO<T>::~PSO() = default;

template <typename T>
const std::vector<typename PSO<T>::Particle> &PSO<T>::getParticles() { return particles_; }

template <typename T>
void PSO<T>::initParticles(const std::vector<T> &ul_ini_bound, const std::vector<T> &ll_ini_bound) {
    for (int i = 0; i < particles_.size(); ++i) {
        Particle *p = &particles_[i];
        for (int d = 0; d < num_dim_; ++d) {
            T ul = ul_ini_bound[d];
            T ll = ll_ini_bound[d];
            std::uniform_real_distribution<T> dist(ll, ul);
            p->position[d]      = dist(rng_);
            p->best_position[d] = p->position[d];
        }
    }
}

template <typename T>
void PSO<T>::setParticles(const std::vector<std::vector<T>> &positions, const std::vector<std::vector<T>> &velocities) {
    for (int i = 0; i < particles_.size(); ++i) {
        particles_[i].position = positions[i];
        particles_[i].velocity = velocities[i];
    }
}

template <typename T>
void PSO<T>::setEvalData(const int &n, const std::vector<std::pair<T, T>> &eval_data) {
    particles_[n].eval_data = eval_data;
    return;
}

template <typename T>
typename Optimizer::Stats<T> PSO<T>::calcPersonalScore(const int &n) {
    typename Optimizer::Stats<T> stats = Optimizer::computeStats(particles_[n].eval_data);
    particles_[n].my_stats = stats;
    return stats;
}

template <typename T>
void PSO<T>::updatePersonalBest(const int &n, const T &score) {
    particles_[n].my_score = score;
    if (particles_[n].my_score < particles_[n].best_score) {
        particles_[n].best_score    = particles_[n].my_score;
        particles_[n].best_position = particles_[n].position;
        particles_[n].best_stats    = particles_[n].my_stats;
    }
    return;
}

template <typename T>
void PSO<T>::updateGlobalBest() {
    for (auto &p : particles_) {
        if (p.best_score < gbest_.score) {
            gbest_.score        = p.best_score;
            gbest_.position     = p.best_position;
            gbest_.num_particle = p.no;
            gbest_.stats        = p.best_stats;
        }
    }
}

template <typename T>
const typename PSO<T>::Gbest &PSO<T>::getGlobalBest() { return gbest_; }

template <typename T>
void PSO<T>::updateParticles() {
    std::uniform_real_distribution<T> dist(0.0, 1.0);
    for (auto &p : particles_) {
        for (int d = 0; d < num_dim_; ++d) {
            T r1 = dist(rng_);
            T r2 = dist(rng_);
            p.velocity[d] = w_[d] * p.velocity[d] 
                          + c1_[d] * r1 * (p.best_position[d] - p.position[d]) 
                          + c2_[d] * r2 * (gbest_.position[d] - p.position[d]);
        }
    }
    for (auto &p : particles_) {
        for (int d = 0; d < num_dim_; ++d) {
            p.position[d] += p.velocity[d]; // 粒子の位置を更新
        }
    }
}

template <typename T>
void PSO<T>::printParticles(int n) {
    if (n > 0) {
        std::cout << "n: " << n << "  pos[0]:" << particles_[n].position[0];
        for (int j = 1; j < particles_[n].position.size(); ++j)
            std::cout << " | " << particles_[n].position[j];
        std::cout << " best_pos[0]: " << particles_[n].best_position[0];
        std::cout << "best_score: " << particles_[n].best_score << std::endl;
    } else {
        for (int i = 0; i < particles_.size(); ++i) {
            std::cout << "i:" << i << " pos[0]:" << particles_[i].position[0];
            for (int j = 1; j < particles_[n].position.size(); ++j)
                std::cout << " | " << particles_[n].position[j];
            std::cout << " best_pos[0]: " << particles_[i].best_position[0];
            std::cout << " best_score: " << particles_[i].best_score << std::endl;
        }
    }
}

template class PSO<double>;
