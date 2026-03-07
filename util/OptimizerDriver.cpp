/**
 * @file OptimizerDriver.cpp
 * @brief 最適化 1 回分の実行窓口。PSO/DE/LM を分岐し、Objective で評価。resultWriter があれば最適化後に結果出力。
 */

#include "util/OptimizerDriver.h"
#include "model/IPhysicalModel.h"
#include "model/IProductDataLoader.h"
#include "product/ProductRunner.h"
#include "product/ProductLoadedData.h"
#include "product/ProductRunResult.h"
#include "util/IResultWriter.h"
#include "product/BatchEvaluationHandler.h"
#include "objective/Objective.h"
#include "core/JacobianResult.h"
#include "util/TraceConfig.h"
#include "util/IterationLog.h"
#include "Optimizer/PSO/PSO.h"
#include "Optimizer/DE/DE.h"
#include "Optimizer/LM/LM.h"
#include <fstream>
#include <memory>
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

static std::vector<ProductRunResult> computeProductResults(
    IPhysicalModel& model,
    IProductDataLoader& loader,
    const std::vector<ProductMeta>& products,
    const std::vector<double>& fullParams) {
    std::vector<ProductRunResult> out;
    for (const auto& meta : products) {
        ProductRunResult r;
        r.product_id = meta.product_id;
        std::unique_ptr<ProductLoadedData> data = loader.load(meta);
        if (!data || data->measured.empty()) {
            r.ok = false;
            r.error_message = "Failed to load product data";
            out.push_back(std::move(r));
            continue;
        }
        std::vector<double> predicted = model.run(fullParams, data.get());
        if (predicted.size() != data->measured.size()) {
            r.ok = false;
            r.error_message = "Model output size mismatch";
            out.push_back(std::move(r));
            continue;
        }
        r.measured = data->measured;
        r.predicted = predicted;
        r.residuals.resize(data->measured.size());
        for (size_t i = 0; i < data->measured.size(); ++i)
            r.residuals[i] = data->measured[i] - predicted[i];
        r.ok = true;
        out.push_back(std::move(r));
    }
    return out;
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

    constexpr double LAMBDA_MIN = 1e-10;
    constexpr double LAMBDA_MAX = 1e12;
    constexpr int LM_MAX_TRY = 8;

    std::vector<double> lo = mapper.getLowerBounds(), up = mapper.getUpperBounds();
    std::vector<bool> applyBounds = mapper.getApplyBounds();

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
        const double current_rmse = lm.getStats().rmse;
        bool accepted = false;
        for (int tryStep = 0; tryStep < LM_MAX_TRY && !accepted; ++tryStep) {
            const std::vector<double>& delta = lm.getNextDelta();
            std::vector<double> z_new = z;
            for (size_t i = 0; i < N_DIM; ++i) {
                z_new[i] += delta[i];
                if (TraceConfig::isLmApplyBoundsEnabled() && i < applyBounds.size() && applyBounds[i]
                    && i < lo.size() && i < up.size()) {
                    if (z_new[i] < lo[i]) z_new[i] = lo[i];
                    if (z_new[i] > up[i]) z_new[i] = up[i];
                }
            }
            JacobianResult jr_new = objective.evaluateWithJacobian(z_new);
            if (jr_new.residuals.size() != nData) break;
            double new_rmse = 0.0;
            for (size_t i = 0; i < nData; ++i) new_rmse += jr_new.residuals[i] * jr_new.residuals[i];
            new_rmse = std::sqrt(new_rmse / static_cast<double>(nData));
            if (new_rmse < current_rmse) {
                z = z_new;
                lm.setLambda(std::max(LAMBDA_MIN, lm.getLambda() * 0.5));
                accepted = true;
            } else {
                lm.setLambda(std::min(LAMBDA_MAX, lm.getLambda() * 10.0));
                if (lm.getLambda() >= LAMBDA_MAX) accepted = true;
            }
        }
        if (accepted) {
            JacobianResult jz = objective.evaluateWithJacobian(z);
            for (size_t i = 0; i < nData; ++i) eval_data[i] = {measured[i], measured[i] - jz.residuals[i]};
            lm.setEvalData(eval_data);
        }
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
                         int nIterPSO, int nIterDE, int nIterLM,
                         IResultWriter* resultWriter) {
    ProductRunner runner(model, loader);
    BatchEvaluationHandler batch(runner);
    batch.setProducts(products);
    Objective objective(mapper, batch);

    RunResult result;
    if (optimizerName == "PSO")
        result = runPSO(objective, mapper, tracePath, logLabel, nIterPSO);
    else if (optimizerName == "DE")
        result = runDE(objective, mapper, tracePath, logLabel, nIterDE);
    else if (optimizerName == "LM")
        result = runLM(objective, loader, mapper, products, tracePath, logLabel, nIterLM);
    else
        return result;

    /* USERWORK: resultWriter はオンサイトで IResultWriter を実装したインスタンスを渡す。 */
    if (resultWriter && !result.bestParams.empty()) {
        std::vector<double> fullParams = mapper.expandToFullParameterSet(result.bestParams);
        std::vector<ProductRunResult> presults = computeProductResults(model, loader, products, fullParams);
        resultWriter->writeAfterOptimization(fullParams, presults);
    }
    return result;
}

RunResult OptimizerDriver::run(const std::string& configPath,
                                ParameterMapper& mapper,
                                IPhysicalModel& model,
                                IProductDataLoader& loader,
                                const std::vector<ProductMeta>& products,
                                const std::string& optimizerName,
                                const std::string& tracePath,
                                const char* logLabel,
                                IResultWriter* resultWriter) {
    TraceConfig::load(configPath);
    return runImpl(mapper, model, loader, products, optimizerName, tracePath, logLabel,
                  N_ITER_PSO_DEFAULT, N_ITER_DE_DEFAULT, N_ITER_LM_DEFAULT, resultWriter);
}

RunResult OptimizerDriver::run(const RunConfig& config,
                                ParameterMapper& mapper,
                                IPhysicalModel& model,
                                IProductDataLoader& loader,
                                const std::vector<ProductMeta>& products,
                                const std::string& optimizerName,
                                const std::string& tracePath,
                                const char* logLabel,
                                IResultWriter* resultWriter) {
    TraceConfig::loadFromStruct(config);
    return runImpl(mapper, model, loader, products, optimizerName, tracePath, logLabel,
                  config.n_iter_pso, config.n_iter_de, config.n_iter_lm, resultWriter);
}

/* USERWORK: dbValueProvider で DB から適用値を取得。resultWriter はオンサイト実装で結果を出力する。 */
void OptimizerDriver::runApplyOnly(ParameterMapper& mapper,
                                    IPhysicalModel& model,
                                    IProductDataLoader& loader,
                                    const std::vector<ProductMeta>& products,
                                    DbValueProvider dbValueProvider,
                                    IResultWriter& resultWriter) {
    std::vector<double> fullParams = mapper.getInitialVector(dbValueProvider);
    std::vector<ProductRunResult> results = computeProductResults(model, loader, products, fullParams);
    resultWriter.writeApplyOnly(fullParams, results);
}

}  // namespace optimizer
