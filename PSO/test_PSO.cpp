#include "PSO.h"
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>

// CppUTest includes should be after your and system includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"

// Utility
#include "csvwriter.h"

using std::cout;
using std::endl;

// static const N_DIM = 4;
// static const N_PARTICLE = 10;
enum {
    N_DIM      = 4, // 次元
    N_PARTICLE = 10 // 粒子数
};
std::vector<double> ll_bnd(N_DIM, -1000.0);
std::vector<double> ul_bnd(N_DIM, 1000.0);
std::vector<double> ll_ini_bnd(N_DIM, 0.1);
std::vector<double> ul_ini_bnd(N_DIM, 0.1);

TEST_GROUP(PSO_SOT) {
    PSO<double> *pso;
    void setup() {
        std::vector<double> w(N_DIM, 0.7);
        std::vector<double> c1(N_DIM, 2.0);
        std::vector<double> c2(N_DIM, 2.6);
        pso = new PSO(N_PARTICLE, N_DIM, w, c1, c2, ul_bnd, ll_bnd
                      // ul_ini_bnd, ll_ini_bnd
        );
    }
    void teardown() { delete pso; }
};

TEST_GROUP(PSO_AOT) {
    PSO<double> *pso;
    void setup() {}
    void teardown() { delete pso; }
};

TEST(PSO_SOT, Create) {
    std::vector<PSO<double>::Particle> particles = pso->getParticles();
    LONGS_EQUAL(N_PARTICLE, particles.size());
}

bool areParticlesEqual(const PSO<double>::Particle &, const PSO<double>::Particle &);
TEST(PSO_SOT, Initial) {
    std::vector<PSO<double>::Particle> particles = pso->getParticles();
    LONGS_EQUAL(N_PARTICLE, particles.size());

    // 初期化前の粒子の状態を保存
    std::vector<PSO<double>::Particle> init_particles = particles;

    // 粒子を初期化
    pso->initParticles(ul_ini_bnd, ll_ini_bnd);
    particles = pso->getParticles();

    // 初期化後の粒子の状態が異なることを確認
    bool result = std::equal(particles.begin(), particles.end(), init_particles.begin(), areParticlesEqual);
    CHECK_FALSE(result);

    // 初期化範囲に収まっていることをチェック
    bool result2 = true;
    for (int i = 0; i < N_PARTICLE; ++i) {
        for (int j = 0; j < N_DIM; ++j) {
            if (ll_bnd[j] > particles[i].position[j] || ul_bnd[j] < particles[i].position[j]) {
                result2 = false;
            }
        }
    }
    CHECK(result2);
}

TEST(PSO_SOT, Pbest) {
    std::vector<PSO<double>::Particle> particles(N_PARTICLE);

    std::vector<double> pos(N_DIM, 2.0);
    std::vector<double> vel(N_DIM, 1.0);
    std::vector<std::vector<double>> positions(N_PARTICLE, pos);
    std::vector<std::vector<double>> velocities(N_PARTICLE, vel);

    std::vector<std::pair<double, double>> eval_data(10);
    eval_data = {{1.0, 2.0}, {1.0, 2.0}, {3.0, 4.0}, {1.0, 2.0}, {1.0, 2.0},
                 {1.0, 2.0}, {1.0, 2.0}, {1.0, 2.0}, {1.0, 2.0}, {11.0, 12.0}};

    // pso->setParticles(positions,velocities);
    pso->setEvalData(0, eval_data);
    Optimizer::Stats<double> stats = pso->calcPersonalScore(0);
    DOUBLES_EQUAL(-1.0, stats.mean, 1.0e-9);

    pso->updatePersonalBest(0, stats.mean);
    particles = pso->getParticles();
    DOUBLES_EQUAL(stats.mean, particles[0].best_score, 1.0e-9);
}

TEST(PSO_SOT, UpdateGbest) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(-10.0, 10.0);

    PSO<double>::Gbest gbest;
    std::vector<PSO<double>::Particle> particles(N_PARTICLE);
    std::vector<double> pos(N_DIM, 0.0);
    std::vector<double> vel(N_DIM, 0.0);
    std::vector<std::vector<double>> positions(N_PARTICLE, pos);
    std::vector<std::vector<double>> velocities(N_PARTICLE, vel);

    for (int i = 0; i < N_PARTICLE; ++i) {
        for (int j = 0; j < N_DIM; ++j) {
            pos[j] = dist(rng);
            vel[j] = dist(rng);
        }
        positions[i]  = pos;
        velocities[i] = vel;
    }

    pso->setParticles(positions, velocities);
    for (int i = 0; i < N_PARTICLE; ++i)
        pso->updatePersonalBest(i, (double)(N_PARTICLE - i));

    pso->updateGlobalBest();
    gbest = pso->getGlobalBest();

    DOUBLES_EQUAL(1.0, gbest.score, 1.0e-9);
    LONGS_EQUAL(N_PARTICLE - 1, gbest.num_particle);
    DOUBLES_EQUAL(positions[N_PARTICLE - 1][0], gbest.position[0], 1.0e-9);
}

TEST(PSO_SOT, UpdateParticle) {
    pso->updateParticles();
    // FAIL("Start here...")
}

TEST(PSO_AOT, DEMO) {
    // FAIL("Start here...")

    const int NI    = 21;
    const int NM    = 6;
    const int NR    = 50;
    const int NUMAX = 00; // 更新回数

    ////////////////////////////////////////////////////////////////////////
    // csv用
    int csv_nu[NUMAX];
    double csv_gb_mean[NUMAX];
    double csv_gb_sig[NUMAX];
    double csv_gb_rmse[NUMAX];
    double csv_gb_pos[NUMAX][NM];

    const int NOP        = 5;
    int csv_out_pno[NOP] = {0, 100, 200, 300, 400};
    double csv_p_pos[NUMAX][NOP];
    double csv_bp_pos[NUMAX][NOP];
    double csv_p_rmse[NUMAX][NOP];

    const int NOZ = 100;
    double csv_z5[NUMAX][NOZ];
    double csv_bestz5[NUMAX][NOZ];
    double csv_z5v[NUMAX][NOZ];
    ////////////////////////////////////////////////////////////////////////

    // サンプルデータ
    double x[NI] = {0.00, 0.50, 1.00, 1.50, 2.00, 2.50, 3.00, 3.50, 4.00, 4.50, 5.00,
                    5.50, 6.00, 6.50, 7.00, 7.50, 8.00, 8.50, 9.00, 9.50, 10.00};
    double y[NI] = {180.00, 129.79, 90.47,  66.68,  60.00,  69.51,  92.35, 124.22, 159.99, 194.21, 221.67,
                    237.91, 239.84, 226.20, 198.19, 159.95, 119.15, 87.51, 81.36,  122.18, 237.17};
    //     double y[NI] = {
    // 125.00
    // ,74.79
    // ,35.47
    // ,11.68
    // ,5.00
    // ,14.51
    // ,37.35
    // ,69.22
    // ,104.99
    // ,139.21
    // ,166.67
    // ,182.91
    // ,184.84
    // ,171.20
    // ,143.19
    // ,104.95
    // ,64.15
    // ,32.51
    // ,26.36
    // ,67.18
    // ,182.17
    //     };
    // 初期粒子範囲
    std::vector<double> ll_bnd2(NM, -1000.0);
    std::vector<double> ul_bnd2(NM, 1000.0);
    std::vector<double> ll_ini_bnd2(NM, -1.);
    std::vector<double> ul_ini_bnd2(NM, 1.);

    // PSOパラメータ
    std::vector<double> w(NM, 0.7);
    std::vector<double> c1(NM, 2.0);
    std::vector<double> c2(NM, 0.8);
    std::random_device rd;  // 非決定的な乱数生成器を初期化
    std::mt19937 gen(rd()); // メルセンヌ・ツイスタ生成器を初期化

    // PSOパラメータの初期化 慣性は±10%バラつかせる
    std::uniform_real_distribution<double> dist(0.0, 0.1);
    for (int d = 0; d < NM; ++d) {
        w[d] = w[d] + dist(gen);
    }

    pso = new PSO(NR, NM, w, c1, c2, ll_bnd2, ul_bnd2);
    pso->initParticles(ll_ini_bnd2, ul_ini_bnd2);
    const std::vector<PSO<double>::Particle> &particles = pso->getParticles();

    srand(0u);

    PSO<double>::Gbest gb;
    for (int ik = 0; ik < NUMAX; ++ik) {  // 更新回数ループ
        for (int ip = 0; ip < NR; ++ip) { // 粒子数ループ
            // 目的関数の計算
            double fz[NI] = {0.0};
            std::vector<std::pair<double, double>> eval_data(NI);
            for (int i = 0; i < NI; ++i) { // データ数
                fz[i] =
                    particles[ip].position[0] * pow(x[i], 5.0) + particles[ip].position[1] * pow(x[i], 4.0) +
                    particles[ip].position[2] * pow(x[i], 3.0) + particles[ip].position[3] * pow(x[i], 2.0) +
                    particles[ip].position[4] * x[i] + particles[ip].position[5];
                // fz[i] = particles[ip].position[0] * pow(x[i],2.0)
                //       + particles[ip].position[1] * x[i]
                //       + particles[ip].position[2];

                eval_data[i] = std::make_pair(y[i],fz[i]);
            }
            pso->setEvalData(ip, eval_data);
            Optimizer::Stats<double> stats = pso->calcPersonalScore(ip);
            pso->updatePersonalBest(ip, stats.rmse);
        }
        pso->updateGlobalBest();
        gb          = pso->getGlobalBest();
        double mean = particles[gb.num_particle].best_stats.mean;
        double std  = particles[gb.num_particle].best_stats.std_dev;
        ////////////////////////////////////////////////////////////////////////

                if(ik == 0){
                    cout << endl;
                    cout << std::setprecision(1);
                    cout << std::setw(8) << "update";
                    cout << std::setw(8) << "bpn";
                    cout << std::setw(8) << "ave";
                    cout << std::setw(8) << "sig";
                    cout << std::setw(8) << "RMSE";
                    cout <<"   ﾊﾟﾗﾒｰﾀ 1～6"<< endl;
                }
        ////////////////////////////////////////////////////////////////////////
                cout << std::setw(8) << ik+1;
                cout << std::setw(8) << gb.num_particle;
                cout << std::right << std::fixed;
                cout << std::setw(8) << mean;
                cout << std::setw(8) << std;
                cout << std::setw(8) << gb.score;

                for(int i = 0; i < NM; ++i ) {
                    // cout << std::scientific;
                    cout << std::setprecision(3) << std::setw(10) << gb.position[i];
                    csv_gb_pos[ik][i]  = gb.position[i];
                }
                cout << endl;
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        csv_nu[ik]      = ik + 1;
        csv_gb_mean[ik] = mean;
        csv_gb_sig[ik]  = std;
        csv_gb_rmse[ik] = gb.score;
        for (int ip = 0; ip < NR; ++ip) {

            if (ip < NOZ) {
                csv_z5[ik][ip]     = particles[ip].position[5];
                csv_bestz5[ik][ip] = particles[ip].best_position[5];
                csv_z5v[ik][ip]    = particles[ip].velocity[5];
            }

            bool out_exec = false;
            int ic        = 0;
            int idx       = 0;
            for (ic = 0; ic < NOP; ++ic) {
                if (ip == csv_out_pno[ic]) {
                    out_exec = true;
                    break;
                    idx = csv_out_pno[ic];
                }
            }
            if (!out_exec)
                continue;

            csv_p_pos[ik][ic]  = particles[ip].position[5];
            csv_bp_pos[ik][ic] = particles[ip].best_position[5];
            csv_p_rmse[ik][ic] = particles[ip].my_score;
        }
        ////////////////////////////////////////////////////////////////////////

        pso->updateParticles();
    }
    // constexpr std::size_t Rows = NUMAX;
    // CSVWriter<Rows> writer("output.csv");
    // writer.write("ik", csv_nu, "%d");
    // // writer.write("mean", csv_gb_mean, "%f");
    // // writer.write("sig", csv_gb_sig, "%f");
    // writer.write("rmse", csv_gb_rmse, "%f");
    // writer.write("z",csv_gb_pos,"%f");
    // writer.out();

    // CSVWriter<Rows> particle("particle.csv");
    // particle.write("ik", csv_nu, "%d");
    // particle.write("ppos",csv_p_pos,"%f");
    // particle.write("best_ppos",csv_bp_pos,"%f");
    // particle.write("z",csv_gb_pos,"%f");
    // particle.write("gbrmse", csv_gb_rmse, "%f");
    // particle.write("rmse",csv_p_rmse,"%f");
    // particle.out();

    // CSVWriter<Rows> z2("z5.csv");
    // z2.write("ik", csv_nu, "%d");
    // z2.write("z5", csv_z5, "%f");
    // z2.write("bz5", csv_bestz5, "%f");
    // z2.out();

    // CSVWriter<Rows> vel("vel.csv");
    // vel.write("ik", csv_nu, "%d");
    // vel.write("z5vel", csv_z5v, "%f");
    // vel.out();
}

// int main(int argc, char** argv){
//     return CommandLineTestRunner::RunAllTests(argc, argv);
// }

bool areParticlesEqual(const PSO<double>::Particle &p1, const PSO<double>::Particle &p2) {
    return p1.position == p2.position && p1.velocity == p2.velocity && p1.best_position == p2.best_position &&
           std::fabs(p1.best_score - p2.best_score) < 1e-9;
}
