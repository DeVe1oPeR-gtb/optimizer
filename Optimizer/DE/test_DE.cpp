#include "Optimizer/DE/DE.h"
#include <algorithm>
#include <iostream>

#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"

using std::cout;
using std::endl;

enum {
    N_DIM      = 4,
    N_POPULATION = 10
};
std::vector<double> ll_bnd_de(N_DIM, -1000.0);
std::vector<double> ul_bnd_de(N_DIM, 1000.0);
std::vector<double> ll_ini_bnd_de(N_DIM, 0.1);
std::vector<double> ul_ini_bnd_de(N_DIM, 0.1);

TEST_GROUP(DE_SOT) {
    DE<double> *de;
    void setup() {
        de = new DE<double>(N_POPULATION, N_DIM, 0.5, 0.9, ul_bnd_de, ll_bnd_de);
    }
    void teardown() { delete de; }
};

TEST_GROUP(DE_AOT) {
    DE<double> *de;
    void setup() {
        
    }
    void teardown() { delete de; }
};

TEST(DE_SOT, Create) {
    std::vector<DE<double>::Individual> population = de->getPopulation();
    LONGS_EQUAL(N_POPULATION, population.size());
}

TEST(DE_SOT, Initial) {
    std::vector<DE<double>::Individual> population = de->getPopulation();
    LONGS_EQUAL(N_POPULATION, population.size());

    // 初期化前の個体の状態を保存
    std::vector<DE<double>::Individual> init_population = population;

    // 個体を初期化
    de->initPopulation(ul_ini_bnd_de, ll_ini_bnd_de);
    population = de->getPopulation();

    // 初期化後の個体の状態が異なることを確認
    bool result = std::equal(population.begin(), population.end(), init_population.begin(), [](const DE<double>::Individual &a, const DE<double>::Individual &b) {
        return a.position == b.position;
    });
    CHECK_FALSE(result);

    // 初期化範囲に収まっていることをチェック
    bool result2 = true;
    for (int i = 0; i < N_POPULATION; ++i) {
        for (int j = 0; j < N_DIM; ++j) {
            if (ll_bnd_de[j] > population[i].position[j] || ul_bnd_de[j] < population[i].position[j]) {
                result2 = false;
            }
        }
    }
    CHECK(result2);
}

TEST(DE_AOT, DEMO) {
    const int NI    = 21;
    const int NM    = 6;
    const int NR    = 50;
    const int NUMAX = 1000; // 更新回数

    // サンプルデータ
    double x[NI] = {0.00, 0.50, 1.00, 1.50, 2.00, 2.50, 3.00, 3.50, 4.00, 4.50, 5.00,
                    5.50, 6.00, 6.50, 7.00, 7.50, 8.00, 8.50, 9.00, 9.50, 10.00};
    double y[NI] = {180.00, 129.79, 90.47,  66.68,  60.00,  69.51,  92.35, 124.22, 159.99, 194.21, 221.67,
                    237.91, 239.84, 226.20, 198.19, 159.95, 119.15, 87.51, 81.36,  122.18, 237.17};

    // 初期個体範囲
    std::vector<double> ll_bnd2_de(NM, -1000.0);
    std::vector<double> ul_bnd2_de(NM, 1000.0);
    std::vector<double> ll_ini_bnd2_de(NM, -10.);
    std::vector<double> ul_ini_bnd2_de(NM, 10.1);

    // DEパラメータ
    double F = 0.5;
    double CR = 0.9;

    de = new DE<double>(NR, NM, F, CR, ul_bnd2_de, ll_bnd2_de);
    de->initPopulation(ul_ini_bnd2_de, ll_ini_bnd2_de);
    const std::vector<DE<double>::Individual>& population = de->getPopulation();
    const std::vector<DE<double>::Individual>& mutants = de->getMutants();

    srand(0u);

    for (int ik = 0; ik < NUMAX; ++ik) {  // 更新回数ループ
        for (int ip = 0; ip < NR; ++ip) { // 個体数ループ
            // 目的関数の計算
            double fz[NI] = {0.0};
            std::vector<std::pair<double, double>> eval_data(NI);
            for (int i = 0; i < NI; ++i) { // データ数
                fz[i] =
                    mutants[ip].position[0] * pow(x[i], 5.0) + mutants[ip].position[1] * pow(x[i], 4.0) +
                    mutants[ip].position[2] * pow(x[i], 3.0) + mutants[ip].position[3] * pow(x[i], 2.0) +
                    mutants[ip].position[4] * x[i] + mutants[ip].position[5];

                eval_data[i] = std::make_pair(y[i], fz[i]);
            }
            de->setEvalData(ip, eval_data);
            Optimizer::Stats<double> stats = de->calcScore(ip);
            // mutants[ip].score = stats.rmse;
        }
        de->selection();
        if(ik == NUMAX - 1) break;
        de->mutation();

        // 途中結果の表示
        const DE<double>::Individual &best_individual = *std::min_element(population.begin(), population.end(), [](const DE<double>::Individual &a, const DE<double>::Individual &b) {
            return a.score < b.score;
        });

        if (ik == 0) {
            cout << "Iteration, Best individual score, Best individual position" << endl;
        }
        cout << ik << ", " << best_individual.score << ", ";
        for (const auto &pos : best_individual.position) {
            cout << pos << " ";
        }
        cout << endl;
    }

    // 最終結果の表示
    const DE<double>::Individual &best_individual = *std::min_element(population.begin(), population.end(), [](const DE<double>::Individual &a, const DE<double>::Individual &b) {
        return a.score < b.score;
    });

    cout << "Best individual score: " << best_individual.score << endl;
    cout << "Best individual position: ";
    for (const auto &pos : best_individual.position) {
        cout << pos << " ";
    }
    cout << endl;
}
