#include "PSO.h"
#include <iomanip>
PSO::PSO( // コンストラクタ
    int num_particles, 
    int num_dim_,
    std::vector<double> w_,  // 慣性
    std::vector<double> c1_, // Pbest重み
    std::vector<double> c2_, // Gbest重み
    std::vector<double> ul_bound_,
    std::vector<double> ll_bound_)
    : Optimizer(), // 基底クラスのコンストラクタを明示的に呼ぶ
      num_dim_(num_dim_), w_(w_), c1_(c1_), c2_(c2_), ul_bound_(ul_bound_), ll_bound_(ll_bound_),

      rng_(std::random_device{}()) {

    // 粒子の初期化
    particles_.resize(num_particles);
    for (int i = 0; i < particles_.size(); ++i) {
        PSO::Particle *p = &particles_[i];
        p->position.resize(num_dim_);
        p->velocity.resize(num_dim_, 0.0); // 速度は0に初期化
        p->best_position.resize(num_dim_);
        for (int d = 0; d < num_dim_; ++d) {
            double ul = ul_bound_[d];
            double ll = ll_bound_[d];
            std::uniform_real_distribution<double> dist(ll, ul);
            p->position[d]      = dist(rng_);
            p->best_position[d] = p->position[d];
            p->my_stats.initialize();
            p->best_stats.initialize();
            p->no = i;
        }
        p->my_score   = std::numeric_limits<double>::max();
        p->best_score = std::numeric_limits<double>::max();
    }

    // PSO群のグローバルベスト情報の初期化
    gbest_.num_particle = 0;
    gbest_.position.resize(num_dim_);
    gbest_.position = particles_[0].position;
    gbest_.score    = std::numeric_limits<double>::max();
}
PSO::~PSO() = default;

const std::vector<PSO::Particle> &PSO::getParticles() { return particles_; }

void PSO::initParticles(const std::vector<double> &ul_ini_bound, const std::vector<double> &ll_ini_bound) {
    for (int i = 0; i < particles_.size(); ++i) {
        // auto &p, &ul, &ll: particles_, ul_ini_bound_, ll_ini_bound_){
        PSO::Particle *p = &particles_[i];
        for (int d = 0; d < num_dim_; ++d) {
            double ul = ul_ini_bound[d];
            double ll = ll_ini_bound[d];
            std::uniform_real_distribution<double> dist(ll, ul);
            p->position[d]      = dist(rng_);
            p->best_position[d] = p->position[d];
        }
    }
}

void PSO::setParticles(const std::vector<std::vector<double>> &positions,
                       const std::vector<std::vector<double>> &velocities) {
    for (int i = 0; i < particles_.size(); ++i) {
        particles_[i].position = positions[i];
        particles_[i].velocity = velocities[i];
    }
}

void PSO::setEvalData(const int &n, const std::vector<std::pair<double, double>> &eval_data) {
    particles_[n].eval_data = eval_data;
    return;
}

Optimizer::Stats PSO::calcPersonalScore(const int &n) {
    Optimizer::Stats stats = Optimizer::computeStats(particles_[n].eval_data);
    particles_[n].my_stats = stats;
    return stats;
}

void PSO::updatePersonalBest(const int &n, const double &score) {
    particles_[n].my_score = score;
    if (particles_[n].my_score < particles_[n].best_score) {
        particles_[n].best_score    = particles_[n].my_score;
        particles_[n].best_position = particles_[n].position;
        particles_[n].best_stats    = particles_[n].my_stats;
    }
    // PSO::printParticles(n);
    return;
}

void PSO::updateGlobalBest() {
    for (auto &p : particles_) {
        if (p.best_score < gbest_.score) {
            gbest_.score        = p.best_score;
            gbest_.position     = p.best_position;
            gbest_.num_particle = p.no;
            gbest_.stats        = p.best_stats;
        }
    }
}

const PSO::Gbest &PSO::getGlobalBest() { return gbest_; }

void PSO::updateParticles() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (auto &p : particles_) {
        for (int d = 0; d < num_dim_; ++d) {
            double r1 = dist(rng_);
            double r2 = dist(rng_);
            // double r1 = (double)(rand()/RAND_MAX);
            // double r2 = (double)(rand()/RAND_MAX);
            // PSO法による粒子の移動 [TODO] w_,c1,c2をベクトル化する
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

void PSO::printParticles(int n) {
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
