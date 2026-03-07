#include "CppUTest/TestHarness.h"
#include "core/IObjective.h"
#include "core/EvalResult.h"
#include "PSO/PSO.h"
#include "DE/DE.h"
#include <vector>
#include <cmath>

namespace {
class DummyObjective : public optimizer::IObjective {
public:
    optimizer::EvalResult evaluate(const std::vector<double>& x) override {
        optimizer::EvalResult r;
        double ss = 0.0;
        for (double v : x) ss += v * v;
        r.objective = std::sqrt(ss);
        r.residuals = x;
        return r;
    }
};
}

TEST_GROUP(OptimizerConnection) {};

TEST(OptimizerConnection, PSOWithDummyObjective) {
    const int N_DIM = 2, N_PARTICLE = 4;
    std::vector<double> w(N_DIM, 0.7), c1(N_DIM, 2.0), c2(N_DIM, 2.0);
    std::vector<double> ll(N_DIM, -10.0), ul(N_DIM, 10.0);
    PSO<double> pso(N_PARTICLE, N_DIM, w, c1, c2, ul, ll);
    DummyObjective obj;
    std::vector<double> init_lo(N_DIM, 0.0), init_up(N_DIM, 1.0);
    pso.initParticles(init_up, init_lo);
    for (int iter = 0; iter < 2; ++iter) {
        const auto& particles = pso.getParticles();
        for (int i = 0; i < N_PARTICLE; ++i) {
            auto res = obj.evaluate(particles[i].position);
            std::vector<std::pair<double, double>> ed = {{res.objective, 0.0}};
            pso.setEvalData(i, ed);
            auto stats = pso.calcPersonalScore(i);
            pso.updatePersonalBest(i, stats.rmse);
        }
        pso.updateGlobalBest();
        pso.updateParticles();
    }
    auto gb = pso.getGlobalBest();
    CHECK(gb.score >= 0.0);
}

TEST(OptimizerConnection, DEWithDummyObjective) {
    const int N_DIM = 2, N_POP = 4;
    std::vector<double> ll(N_DIM, -10.0), ul(N_DIM, 10.0);
    DE<double> de(N_POP, N_DIM, 0.5, 0.9, ul, ll);
    std::vector<double> init_lo(N_DIM, 0.0), init_up(N_DIM, 1.0);
    de.initPopulation(init_up, init_lo);
    DummyObjective obj;
    for (int iter = 0; iter < 2; ++iter) {
        const auto& mutants = de.getMutants();
        for (int i = 0; i < N_POP; ++i) {
            auto res = obj.evaluate(mutants[i].position);
            std::vector<std::pair<double, double>> ed(res.residuals.size());
            for (size_t j = 0; j < res.residuals.size(); ++j)
                ed[j] = {res.residuals[j], 0.0};
            de.setEvalData(i, ed);
            de.calcScore(i);
        }
        de.selection();
        if (iter < 1) de.mutation();
    }
    const auto& pop = de.getPopulation();
    CHECK(pop.size() == static_cast<size_t>(N_POP));
}
