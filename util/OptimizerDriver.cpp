/**
 * @file OptimizerDriver.cpp
 * @brief 最適化 1 回分の実行窓口。PSO/DE/LM を分岐し、Objective で評価。resultWriter があれば最適化後に結果出力。
 */

#include "util/OptimizerDriver.hpp"
#include "model/IPhysicalModel.hpp"
#include "model/IProductDataLoader.hpp"
#include "product/ProductRunner.hpp"
#include "product/ProductLoadedData.hpp"
#include "product/ProductRunResult.hpp"
#include "util/IResultWriter.hpp"
#include "product/BatchEvaluationHandler.hpp"
#include "objective/Objective.hpp"
#include "core/core.hpp"
#include "util/TraceConfig.hpp"
#include "util/IterationLog.hpp"
#include "util/LogRotate.hpp"
#include "Optimizer/PSO/PSO.hpp"
#include "Optimizer/DE/DE.hpp"
#include "Optimizer/LM/LM.hpp"
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <cmath>

namespace optimizer {

/** debug=on のとき、オンサイトで必要な入力が揃っているかチェックしログ出力する。 */
static void checkRequiredInputs(const std::string& configPath,
                                ParameterMapper& mapper,
                                const std::vector<ProductMeta>& products,
                                const std::string& optimizerName) {
    if (!TraceConfig::isDebugEnabled()) return;

    TraceConfig::logDebug("--- required inputs check ---");

    if (!configPath.empty()) {
        std::ifstream f(configPath);
        if (f)
            TraceConfig::logDebug("config file: OK (" + configPath + ")");
        else
            TraceConfig::logDebug("config file: NG (not found: " + configPath + ")");
    } else {
        TraceConfig::logDebug("config file: (using RunConfig, no file)");
    }

    const size_t nSpecs = mapper.specs().size();
    if (nSpecs > 0)
        TraceConfig::logDebug("ParameterMapper specs: " + std::to_string(nSpecs) + " params");
    else
        TraceConfig::logDebug("ParameterMapper specs: NG (empty)");

    std::string err;
    if (mapper.validate(err))
        TraceConfig::logDebug("ParameterMapper validate: OK");
    else
        TraceConfig::logDebug("ParameterMapper validate: NG (" + err + ")");

    const size_t nOpt = mapper.numOptParams();
    const size_t nFull = mapper.numFullParams();
    TraceConfig::logDebug("ParameterMapper numOptParams=" + std::to_string(nOpt) + " numFullParams=" + std::to_string(nFull));

    std::vector<double> z0 = mapper.getInitialVector(nullptr);
    if (z0.size() == nFull)
        TraceConfig::logDebug("initial vector: OK (size=" + std::to_string(z0.size()) + ")");
    else
        TraceConfig::logDebug("initial vector: NG (size=" + std::to_string(z0.size()) + " expected " + std::to_string(nFull) + ")");

    if (!products.empty())
        TraceConfig::logDebug("products: OK (count=" + std::to_string(products.size()) + ")");
    else
        TraceConfig::logDebug("products: NG (empty)");

    if (optimizerName == "PSO" || optimizerName == "DE" || optimizerName == "LM")
        TraceConfig::logDebug("optimizer name: OK (" + optimizerName + ")");
    else
        TraceConfig::logDebug("optimizer name: NG (unknown: " + optimizerName + ")");

    TraceConfig::logDebug("--- end check ---");
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
    const int nParticle = TraceConfig::getPsoNParticle();
    const double initRad = TraceConfig::getPsoInitRadius();
    std::vector<double> lo = mapper.getLowerBounds(), up = mapper.getUpperBounds();
    std::vector<double> z0 = mapper.getInitialVector(nullptr);
    std::vector<double> w(N_DIM, TraceConfig::getPsoW());
    std::vector<double> c1(N_DIM, TraceConfig::getPsoC1());
    std::vector<double> c2(N_DIM, TraceConfig::getPsoC2());
    PSO<double> pso(nParticle, static_cast<int>(N_DIM), w, c1, c2, up, lo);
    std::vector<double> initLo(N_DIM), initUp(N_DIM);
    for (size_t i = 0; i < N_DIM; ++i) {
        initLo[i] = z0[i] - initRad;
        initUp[i] = z0[i] + initRad;
    }
    pso.initParticles(initUp, initLo);

    std::ofstream traceFile;
    std::ofstream particleTraceFile;
    if (TraceConfig::isTraceEnabled() && !tracePath.empty()) {
        const size_t maxBytes = TraceConfig::getTraceLogMaxBytes();
        if (openLogWithRotation(tracePath, traceFile, maxBytes)) {
            traceFile << "iteration,score,p0,p1,p2\n";
            pso.setTraceStream(&traceFile);
            pso.setTraceEnabled(true);
        }
        std::string particleTracePath = tracePath;
        auto pos = particleTracePath.rfind("_trace.");
        if (pos != std::string::npos)
            particleTracePath.replace(pos, 7, "_particle_trace.");
        else if (particleTracePath.rfind(".csv") != std::string::npos)
            particleTracePath.insert(particleTracePath.size() - 4, "_particle");
        else
            particleTracePath += "_particle_trace.csv";
        if (openLogWithRotation(particleTracePath, particleTraceFile, maxBytes)) {
            particleTraceFile << "iteration,particle_id,score";
            for (size_t d = 0; d < N_DIM; ++d) particleTraceFile << ",p" << d;
            particleTraceFile << "\n";
            pso.setParticleTraceStream(&particleTraceFile);
            pso.setParticleTraceEnabled(true);
        }
    }

    for (int iter = 0; iter < nIter; ++iter) {
        const auto& particles = pso.getParticles();
        for (int i = 0; i < nParticle; ++i) {
            auto res = objective.evaluate(particles[i].position);
            std::vector<std::pair<double, double>> ed = {{res.objective, 0.0}};
            pso.setEvalData(i, ed);
            auto stats = pso.calcPersonalScore(i);
            pso.updatePersonalBest(i, stats.rmse);
        }
        pso.updateGlobalBest();
        auto gb = pso.getGlobalBest();
        logIterationWithStats(iter, gb.stats.mean, gb.stats.rmse, gb.position, logLabel);
        pso.writeTraceLine(iter);
        pso.writeParticleTraceLine(iter);
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
    const int nPop = TraceConfig::getDeNPop();
    const double initRad = TraceConfig::getDeInitRadius();
    std::vector<double> lo = mapper.getLowerBounds(), up = mapper.getUpperBounds();
    std::vector<double> z0 = mapper.getInitialVector(nullptr);
    DE<double> de(nPop, static_cast<int>(N_DIM), TraceConfig::getDeF(), TraceConfig::getDeCr(), up, lo);
    std::vector<double> initLo(N_DIM), initUp(N_DIM);
    for (size_t i = 0; i < N_DIM; ++i) {
        initLo[i] = z0[i] - initRad;
        initUp[i] = z0[i] + initRad;
    }
    de.initPopulation(initUp, initLo);

    std::ofstream traceFile;
    if (TraceConfig::isTraceEnabled() && !tracePath.empty()) {
        if (openLogWithRotation(tracePath, traceFile, TraceConfig::getTraceLogMaxBytes())) {
            traceFile << "iteration,score,p0,p1,p2\n";
            de.setTraceStream(&traceFile);
            de.setTraceEnabled(true);
        }
    }

    for (int iter = 0; iter < nIter; ++iter) {
        const auto& mutants = de.getMutants();
        for (int i = 0; i < nPop; ++i) {
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
        logIterationWithStats(iter, best->stats.mean, best->stats.rmse, best->position, logLabel);
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

    LM<double> lm(static_cast<int>(N_DIM), static_cast<int>(nData), z, TraceConfig::getLmRPerturb());
    lm.setLambda(TraceConfig::getLmLambdaInit());
    std::ofstream traceFile;
    if (TraceConfig::isTraceEnabled() && !tracePath.empty()) {
        if (openLogWithRotation(tracePath, traceFile, TraceConfig::getTraceLogMaxBytes())) {
            traceFile << "iteration,rmse,p0,p1,p2\n";
            lm.setTraceStream(&traceFile);
            lm.setTraceEnabled(true);
        }
    }

    const double lambdaMin = TraceConfig::getLmLambdaMin();
    const double lambdaMax = TraceConfig::getLmLambdaMax();
    const double lambdaDown = TraceConfig::getLmLambdaDown();
    const double lambdaUp = TraceConfig::getLmLambdaUp();
    const int lmMaxTry = TraceConfig::getLmMaxTry();

    std::vector<double> lo = mapper.getLowerBounds(), up = mapper.getUpperBounds();
    std::vector<bool> applyBounds = mapper.getApplyBounds();

    for (int iter = 0; iter < nIter; ++iter) {
        JacobianResult jr = objective.evaluateWithJacobian(z);
        if (jr.residuals.size() != nData) break;
        std::vector<std::pair<double, double>> eval_data(nData);
        for (size_t i = 0; i < nData; ++i)
            eval_data[i] = {measured[i], measured[i] - jr.residuals[i]};
        lm.setEvalData(eval_data);
        for (size_t d = 0; d < N_DIM; ++d) {
            std::vector<double> col(nData);
            for (size_t i = 0; i < nData; ++i) col[i] = jr.jacobian[i][d];
            lm.setJacobian(static_cast<int>(d), col);
        }
        const double current_rmse = lm.getStats().rmse;
        bool accepted = false;
        for (int tryStep = 0; tryStep < lmMaxTry && !accepted; ++tryStep) {
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
                lm.setLambda(std::max(lambdaMin, lm.getLambda() * lambdaDown));
                accepted = true;
            } else {
                lm.setLambda(std::min(lambdaMax, lm.getLambda() * lambdaUp));
                if (lm.getLambda() >= lambdaMax) accepted = true;
            }
        }
        if (accepted) {
            JacobianResult jz = objective.evaluateWithJacobian(z);
            for (size_t i = 0; i < nData; ++i) eval_data[i] = {measured[i], measured[i] - jz.residuals[i]};
            lm.setEvalData(eval_data);
        }
        lm.writeTraceLine(iter, z);
        logIterationWithStats(iter, lm.getStats().mean, lm.getStats().rmse, z, logLabel);
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
    if (TraceConfig::isDebugEnabled())
        checkRequiredInputs(configPath, mapper, products, optimizerName);
    return runImpl(mapper, model, loader, products, optimizerName, tracePath, logLabel,
                  TraceConfig::getNIterPso(), TraceConfig::getNIterDe(), TraceConfig::getNIterLm(),
                  resultWriter);
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
    if (TraceConfig::isDebugEnabled())
        checkRequiredInputs("", mapper, products, optimizerName);
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
