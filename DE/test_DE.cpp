#include "DE.h"
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
std::vector<double> ll_bnd(N_DIM, 0.0);
std::vector<double> ul_bnd(N_DIM, 200.0);
std::vector<double> ll_ini_bnd(N_DIM, 0.1);
std::vector<double> ul_ini_bnd(N_DIM, 100.0);

TEST_GROUP(DE_SOT) {
    DE<double> *de;
    void setup() {
        de = new DE<double>(N_POPULATION, N_DIM, 0.5, 0.9, ul_bnd, ll_bnd);
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
    de->initPopulation(ul_ini_bnd, ll_ini_bnd);
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
            if (ll_bnd[j] > population[i].position[j] || ul_bnd[j] < population[i].position[j]) {
                result2 = false;
            }
        }
    }
    CHECK(result2);
}

int main(int argc, char** argv){
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
