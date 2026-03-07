#include "util/OptimizerDriver.h"
#include "model/IPhysicalModel.h"
#include "model/IProductDataLoader.h"
#include "product/ProductRunner.h"
#include "product/BatchEvaluationHandler.h"
#include "objective/Objective.h"
#include "core/JacobianResult.h"
#include "util/TraceConfig.h"
#include "util/IterationLog.h"
#include "Optimizer/PSO/PSO.h"
#include "Optimizer/DE/DE.h"
#include "Optimizer/LM/LM.h"
#include <fstream>
#include <vector>
#include <cmath>

namespace optimizer {

namespace {
const int N_PARTICLE = 20;
const int N_POP_DE = 20;
const int N_ITER_PSO_DEFAULT = 120;
const int N_ITER_DE_DEFAULT = 120;
const int N_ITER_LM_DEFAULT = 80;
}

static RunResult runPSO(Objective& objective,
                        ParameterMapper& mapper,
                        const std::string& tracePath,
                        const char* logLabel,
                        int nIter) {
    const size_t N_DIM = mapper.numOptParams();
    std::vector<double> lo = mapper.getLowerBounds(), up = mapper.getUpperBounds();
    std::vector<double> z0 = mapper.getInitialVector(nullptr);
    std::vector<double> w(N_DIM, 0.7), c1(N_DIM, 1.8), c2(N_DIM, 1.8);
    PSO<double> pso(N_PARTICLE, static_cast<int>(N_DIM), w, c1, c2, up, lo);
    std::vector<double> initLo(N_DIM), initUp(N_DIM);
    for (size_t i = 0; i < N_DIM; ++i) {
        initLo[i] = z0[i] - 0.5;
        initUp[i] = z0[i] + 0.5;
    }
    pso.initParticles(initUp, initLo);

    std::ofstream traceFile;
    if (TraceConfig::isTraceEnabled() && !tracePath.empty()) {
        traceFile.open(tracePath);
        if (traceFile) {
            traceFile << "iteration,score,p0,p1,p2\n";
            pso.setTraceStream(&traceFile);
            pso.setTraceEnabled(true);
        }
    }

    for (int iter = 0; iter < nIter; ++iter) {
        const auto& particles = pso.getParticles();
        for (int i = 0; i < N_PARTICLE; ++i) {
            auto res = objective.evaluate(particles[i].position);
            std::vector<std::pair<double, double>> ed = {{res.objective, 0.0}};
            pso.setEvalData(i, ed);
            auto stats = pso.calcPersonalScore(i);
            pso.updatePersonalBest(i, stats.rmse);
        }
        pso.updateGlobalBest();
        auto gb = pso.getGlobalBest();
        logIteration(iter, gb.score, gb.position, logLabel);
        pso.writeTraceLine(iter);
        pso.updateParticles();
    }

    auto gb = pso.getGlobalBest();
    RunResult out;
    out.bestScore = gb.score;
    out.bestParams = gb.position;
    return out;
}

static RunResult runDE(Objective& objective,
                       ParameterMapper& mapper,
                       const std::string& tracePath,
                       const char* logLabel,
                       int nIter) {
    const size_t N_DIM = mapper.numOptParams();
    std::vector<double> lo = mapper.getLowerBounds(), up = mapper.getUpperBounds();
    std::vector<double> z0 = mapper.getInitialVector(nullptr);
    DE<double> de(N_POP_DE, static_cast<int>(N_DIM), 0.5, 0.9, up, lo);
    std::vector<double> initLo(N_DIM), initUp(N_DIM);
    for (size_t i = 0; i < N_DIM; ++i) {
        initLo[i] = z0[i] - 0.5;
        initUp[i] = z0[i] + 0.5;
    }
    de.initPopulation(initUp, initLo);

    std::ofstream traceFile;
    if (TraceConfig::isTraceEnabled() && !tracePath.empty()) {
        traceFile.open(tracePath);
        if (traceFile) {
            traceFile << "iteration,score,p0,p1,p2\n";
            de.setTraceStream(&traceFile);
            de.setTraceEnabled(true);
        }
    }

    for (int iter = 0; iter < nIter; ++iter) {
        const auto& mutants = de.getMutants();
        for (int i = 0; i < N_POP_DE; ++i) {
            auto res = objective.evaluate(mutants[i].position);
            std::vector<std::pair<double, double>> ed(res.residuals.size());
            for (size_t j = 0; j < res.residuals.size(); ++j) ed[j] = {res.residuals[j], 0.0};
            de.setEvalData(i, ed);
            de.calcScore(i);
        }
        de.writeTraceLine(iter);
        de.selection();
        const auto& pop = de.getPopulation();
        const DE<double>::Individual* best = &pop[0];
        for (const auto& ind : pop)
            if (ind.score < best->score) best = &ind;
        logIteration(iter, best->score, best->position, logLabel);
        if (iter < nIter - 1) de.mutation();
    }

    const auto& pop = de.getPopulation();
    const DE<double>::Individual* best = &pop[0];
    for (const auto& ind : pop)
        if (ind.score < best->score) best = &ind;
    RunResult out;
    out.bestScore = best->score;
    out.bestParams = best->position;
    return out;
}

static RunResult runLM(Objective& objective,
                       IProductDataLoader& loader,
                       ParameterMapper& mapper,
                       const std::vector<ProductMeta>& products,
                       const std::string& tracePath,
                       const char* logLabel,
                       int nIter) {
    const size_t N_DIM = mapper.numOptParams();
    std::vector<double> z = mapper.getInitialVector(nullptr);
    if (products.empty()) return {};
    auto data = loader.load(products[0]);
    if (!data || data->measured.size() != data->positions.size()) return {};
    const size_t nData = data->measured.size();
    std::vector<double> measured = data->measured;

    LM<double> lm(static_cast<int>(N_DIM), static_cast<int>(nData), z);
    std::ofstream traceFile;
    if (TraceConfig::isTraceEnabled() && !tracePath.empty()) {
        traceFile.open(tracePath);
        if (traceFile) {
            traceFile << "iteration,rmse,p0,p1,p2\n";
            lm.setTraceStream(&traceFile);
            lm.setTraceEnabled(true);
        }
    }

    for (int iter = 0; iter < nIter; ++iter) {
        JacobianResult jr = objective.evaluateWithJacobian(z);
        if (jr.residuals.size() != nData) break;
        std::vector<std::pair<double, double>> eval_data(nData);
        for (size_t i = 0; i < nData; ++i)
            eval_data[i] = {measured[i], measured[i] - jr.residuals[i]};
        lm.setEvalData(eval_data);
        for (int d = 0; d < N_DIM; ++d) {
            std::vector<double> col(nData);
            for (size_t i = 0; i < nData; ++i) col[i] = jr.jacobian[i][d];
            lm.setJacobian(d, col);
        }
        const std::vector<double>& delta = lm.getNextDelta();
        for (size_t i = 0; i < N_DIM; ++i) z[i] += delta[i];
        lm.writeTraceLine(iter, z);
        logIteration(iter, lm.getStats().rmse, z, logLabel);
    }

    RunResult out;
    out.bestScore = lm.getStats().rmse;
    out.bestParams = z;
    return out;
}

static RunResult runImpl(ParameterMapper& mapper,
                         IPhysicalModel& model,
                         IProductDataLoader& loader,
                         const std::vector<ProductMeta>& products,
                         const std::string& optimizerName,
                         const std::string& tracePath,
                         const char* logLabel,
                         int nIterPSO, int nIterDE, int nIterLM) {
    ProductRunner runner(model, loader);
    BatchEvaluationHandler batch(runner);
    batch.setProducts(products);
    Objective objective(mapper, batch);

    if (optimizerName == "PSO")
        return runPSO(objective, mapper, tracePath, logLabel, nIterPSO);
    if (optimizerName == "DE")
        return runDE(objective, mapper, tracePath, logLabel, nIterDE);
    if (optimizerName == "LM")
        return runLM(objective, loader, mapper, products, tracePath, logLabel, nIterLM);
    return {};
}

RunResult OptimizerDriver::run(const std::string& configPath,
                                ParameterMapper& mapper,
                                IPhysicalModel& model,
                                IProductDataLoader& loader,
                                const std::vector<ProductMeta>& products,
                                const std::string& optimizerName,
                                const std::string& tracePath,
                                const char* logLabel) {
    TraceConfig::load(configPath);
    return runImpl(mapper, model, loader, products, optimizerName, tracePath, logLabel,
                  N_ITER_PSO_DEFAULT, N_ITER_DE_DEFAULT, N_ITER_LM_DEFAULT);
}

RunResult OptimizerDriver::run(const RunConfig& config,
                                ParameterMapper& mapper,
                                IPhysicalModel& model,
                                IProductDataLoader& loader,
                                const std::vector<ProductMeta>& products,
                                const std::string& optimizerName,
                                const std::string& tracePath,
                                const char* logLabel) {
    TraceConfig::loadFromStruct(config);
    return runImpl(mapper, model, loader, products, optimizerName, tracePath, logLabel,
                  config.n_iter_pso, config.n_iter_de, config.n_iter_lm);
}

}  // namespace optimizer
