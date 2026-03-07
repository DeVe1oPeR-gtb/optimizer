/*
 * デモ用・後で削除。
 * 3種の物理モデルをそれぞれ PSO / DE / LM で最適化し、
 * サマリーは result/、トレースログは trace=on のとき log/ に出力する。
 */

#include "param/ParameterMapper.h"
#include "param/ParamSpec.h"
#include "product/ProductRunner.h"
#include "product/BatchEvaluationHandler.h"
#include "product/ProductMeta.h"
#include "product/ProductLoadedData.h"
#include "objective/Objective.h"
#include "Optimizer/PSO/PSO.h"
#include "Optimizer/DE/DE.h"
#include "Optimizer/LM/LM.h"
#include "Handler/Handler.h"
#include "util/TraceConfig.h"
#include "util/IterationLog.h"
#include "demo/DemoPhysicalModel.h"
#include "demo/DemoPhysicalModel2.h"
#include "demo/DemoPhysicalModel3.h"
#include "demo/DemoDataLoader.h"
#include "demo/DemoDataLoader2.h"
#include "demo/DemoDataLoader3.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/stat.h>

static const int N_DIM = 3;
static const int N_PARTICLE = 20;
static const int N_POP_DE = 20;
static const int N_ITER_PSO = 120;
static const int N_ITER_DE = 120;
static const int N_ITER_LM = 80;

static std::vector<optimizer::ParamSpec> makeSpecs() {
    return {
        {"p0", 1, optimizer::InitMode::Manual, 0.0, "", -5.0, 5.0, ""},
        {"p1", 1, optimizer::InitMode::Manual, 0.0, "", -5.0, 5.0, ""},
        {"p2", 1, optimizer::InitMode::Manual, 0.0, "", -5.0, 5.0, ""},
    };
}

static void ensureResultDir() {
#ifdef _WIN32
    _mkdir("result");
#else
    mkdir("result", 0755);
#endif
}

static void ensureLogDir() {
#ifdef _WIN32
    _mkdir("log");
#else
    mkdir("log", 0755);
#endif
}

static void runPSO(optimizer::Objective& objective, const std::string& tracePath,
                   double& outScore, std::vector<double>& outPos,
                   const char* logLabel = nullptr) {
    optimizer::ParameterMapper mapper;
    mapper.setSpecs(makeSpecs());
    std::vector<double> w(N_DIM, 0.7), c1(N_DIM, 1.8), c2(N_DIM, 1.8);
    std::vector<double> lo = mapper.getLowerBounds(), up = mapper.getUpperBounds();
    PSO<double> pso(N_PARTICLE, N_DIM, w, c1, c2, up, lo);
    std::vector<double> initLo(N_DIM, -0.5), initUp(N_DIM, 0.5);
    pso.initParticles(initUp, initLo);

    bool traceOn = optimizer::TraceConfig::isTraceEnabled();
    std::ofstream traceFile;
    if (traceOn && !tracePath.empty()) {
        traceFile.open(tracePath);
        if (traceFile) {
            traceFile << "iteration,score,p0,p1,p2\n";
            pso.setTraceStream(&traceFile);
            pso.setTraceEnabled(true);
        }
    }

    for (int iter = 0; iter < N_ITER_PSO; ++iter) {
        const auto& particles = pso.getParticles();
        for (int i = 0; i < N_PARTICLE; ++i) {
            auto res = objective.evaluate(particles[i].position);
            std::vector<std::pair<double, double>> ed = {{res.objective, 0.0}};
            pso.setEvalData(i, ed);
            auto stats = pso.calcPersonalScore(i);
            pso.updatePersonalBest(i, stats.rmse);
        }
        pso.updateGlobalBest();
        {
            auto gb = pso.getGlobalBest();
            optimizer::logIteration(iter, gb.score, gb.position, logLabel);
        }
        pso.writeTraceLine(iter);
        pso.updateParticles();
    }

    auto gb = pso.getGlobalBest();
    outScore = gb.score;
    outPos = gb.position;
}

static void runDE(optimizer::Objective& objective, const std::string& tracePath,
                  double& outScore, std::vector<double>& outPos,
                  const char* logLabel = nullptr) {
    optimizer::ParameterMapper mapper;
    mapper.setSpecs(makeSpecs());
    std::vector<double> lo = mapper.getLowerBounds(), up = mapper.getUpperBounds();
    DE<double> de(N_POP_DE, N_DIM, 0.5, 0.9, up, lo);
    std::vector<double> initLo(N_DIM, -0.5), initUp(N_DIM, 0.5);
    de.initPopulation(initUp, initLo);

    bool traceOn = optimizer::TraceConfig::isTraceEnabled();
    std::ofstream traceFile;
    if (traceOn && !tracePath.empty()) {
        traceFile.open(tracePath);
        if (traceFile) {
            traceFile << "iteration,score,p0,p1,p2\n";
            de.setTraceStream(&traceFile);
            de.setTraceEnabled(true);
        }
    }

    for (int iter = 0; iter < N_ITER_DE; ++iter) {
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
        {
            const auto& pop = de.getPopulation();
            const DE<double>::Individual* best = &pop[0];
            for (const auto& ind : pop)
                if (ind.score < best->score) best = &ind;
            optimizer::logIteration(iter, best->score, best->position, logLabel);
        }
        if (iter < N_ITER_DE - 1) de.mutation();
    }

    const auto& pop = de.getPopulation();
    const DE<double>::Individual* best = &pop[0];
    for (const auto& ind : pop)
        if (ind.score < best->score) best = &ind;
    outScore = best->score;
    outPos = best->position;
}

static void runLM(optimizer::Objective& objective, optimizer::IProductDataLoader& loader,
                  const std::string& tracePath, double& outScore, std::vector<double>& outPos,
                  const char* logLabel = nullptr) {
    optimizer::ParameterMapper mapper;
    mapper.setSpecs(makeSpecs());
    std::vector<double> z = mapper.getInitialVector(nullptr);
    optimizer::ProductMeta meta = {"demo1", ""};
    auto data = loader.load(meta);
    if (!data || data->measured.size() != data->positions.size()) return;
    std::vector<double> measured = data->measured;
    const size_t nData = measured.size();

    LM<double> lm(static_cast<int>(N_DIM), static_cast<int>(nData), z);
    bool traceOn = optimizer::TraceConfig::isTraceEnabled();
    std::ofstream traceFile;
    if (traceOn && !tracePath.empty()) {
        traceFile.open(tracePath);
        if (traceFile) {
            traceFile << "iteration,rmse,p0,p1,p2\n";
            lm.setTraceStream(&traceFile);
            lm.setTraceEnabled(true);
        }
    }

    for (int iter = 0; iter < N_ITER_LM; ++iter) {
        optimizer::JacobianResult jr = objective.evaluateWithJacobian(z);
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
        for (int i = 0; i < N_DIM; ++i) z[i] += delta[i];
        lm.writeTraceLine(iter, z);
        optimizer::logIteration(iter, lm.getStats().rmse, z, logLabel);
    }

    outScore = lm.getStats().rmse;
    outPos = z;
}

struct SummaryRow {
    std::string model;
    std::string optimizer;
    double final_rmse;
    double p0, p1, p2;
    int n_iter;
};

int main() {
    Handler handler("config/developer.cfg");
    ensureResultDir();
    if (optimizer::TraceConfig::isTraceEnabled())
        ensureLogDir();

    std::vector<SummaryRow> summary;

    auto runOne = [&summary](int modelId, const std::string& modelName,
                             optimizer::IPhysicalModel& model, optimizer::IProductDataLoader& loader,
                             const std::string& optName,
                             void (*run)(optimizer::Objective&, const std::string&, double&, std::vector<double>&, const char*),
                             int nIter) {
        optimizer::ParameterMapper mapper;
        mapper.setSpecs(makeSpecs());
        optimizer::ProductRunner runner(model, loader);
        optimizer::BatchEvaluationHandler batch(runner);
        batch.setProducts({{"demo1", ""}});
        optimizer::Objective objective(mapper, batch);

        std::string tracePath = "log/model" + std::to_string(modelId) + "_" + optName + "_trace.csv";
        std::string logLabel = modelName + "-" + optName;
        double score = 0;
        std::vector<double> pos;
        run(objective, tracePath, score, pos, logLabel.c_str());

        SummaryRow r;
        r.model = modelName;
        r.optimizer = optName;
        r.final_rmse = score;
        r.p0 = pos.size() > 0 ? pos[0] : 0;
        r.p1 = pos.size() > 1 ? pos[1] : 0;
        r.p2 = pos.size() > 2 ? pos[2] : 0;
        r.n_iter = nIter;
        summary.push_back(r);
    };

    auto runLMOne = [&summary](int modelId, const std::string& modelName,
                                optimizer::IPhysicalModel& model, optimizer::IProductDataLoader& loader) {
        optimizer::ParameterMapper mapper;
        mapper.setSpecs(makeSpecs());
        optimizer::ProductRunner runner(model, loader);
        optimizer::BatchEvaluationHandler batch(runner);
        batch.setProducts({{"demo1", ""}});
        optimizer::Objective objective(mapper, batch);

        std::string tracePath = "log/model" + std::to_string(modelId) + "_LM_trace.csv";
        std::string logLabel = modelName + "-LM";
        double score = 0;
        std::vector<double> pos;
        runLM(objective, loader, tracePath, score, pos, logLabel.c_str());

        SummaryRow r;
        r.model = modelName;
        r.optimizer = "LM";
        r.final_rmse = score;
        r.p0 = pos.size() > 0 ? pos[0] : 0;
        r.p1 = pos.size() > 1 ? pos[1] : 0;
        r.p2 = pos.size() > 2 ? pos[2] : 0;
        r.n_iter = N_ITER_LM;
        summary.push_back(r);
    };

    optimizer::DemoPhysicalModel model1;
    optimizer::DemoDataLoader loader1;
    optimizer::DemoPhysicalModel2 model2;
    optimizer::DemoDataLoader2 loader2;
    optimizer::DemoPhysicalModel3 model3;
    optimizer::DemoDataLoader3 loader3;

    struct ModelEntry { int id; std::string name; optimizer::IPhysicalModel* model; optimizer::IProductDataLoader* loader; };
    std::vector<ModelEntry> models = {
        {1, "quadratic", &model1, &loader1},
        {2, "linear", &model2, &loader2},
        {3, "rational_exp", &model3, &loader3}
    };

    for (const auto& m : models) {
        for (const std::string& opt : handler.getOptimizersToRun()) {
            if (opt == "PSO") runOne(m.id, m.name, *m.model, *m.loader, "PSO", runPSO, N_ITER_PSO);
            else if (opt == "DE") runOne(m.id, m.name, *m.model, *m.loader, "DE", runDE, N_ITER_DE);
            else if (opt == "LM") runLMOne(m.id, m.name, *m.model, *m.loader);
        }
    }

    std::ofstream sumFile("result/summary.csv");
    sumFile << "model,optimizer,final_rmse,p0,p1,p2,n_iter\n";
    for (const auto& r : summary)
        sumFile << r.model << "," << r.optimizer << "," << r.final_rmse << ","
                << r.p0 << "," << r.p1 << "," << r.p2 << "," << r.n_iter << "\n";

    std::cout << "[Demo] " << summary.size() << " runs done. result/summary.csv written.";
    if (optimizer::TraceConfig::isTraceEnabled())
        std::cout << " Trace logs in log/.";
    std::cout << "\n";
    return 0;
}
