/*
 * デモ用・後で削除。
 * OptimizerDriver の利用例: 1 本の ParameterMapper と 3 モデル×最適化器で run を呼ぶだけ。
 */

#include "param/param.hpp"
#include "util/util_common.hpp"
#include "util/ParaConfig.hpp"
#include "util/LogRotate.hpp"
#include "util/OptimizerDriver.hpp"
#include "mock/Demo.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/stat.h>

static std::vector<optimizer::ParamSpec> makeSpecs() {
    return {
        {"p0", 1, optimizer::InitMode::Manual, 1.0, "", -5.0, 5.0, ""},
        {"p1", 1, optimizer::InitMode::Manual, -0.5, "", -5.0, 5.0, ""},
        {"p2", 1, optimizer::InitMode::Manual, 0.1, "", -5.0, 5.0, ""},
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

struct SummaryRow {
    std::string model;
    std::string optimizer;
    double final_rmse;
    double p0, p1, p2;
    int n_iter;
};

static int nIterFor(const std::string& opt) {
    if (opt == "PSO") return 120;
    if (opt == "DE") return 120;
    if (opt == "LM") return 80;
    return 120;
}

int main() {
    const std::string configPath = "config/para.cfg";
    Handler handler(configPath);
    optimizer::DataConfig::load(configPath);
    if (!optimizer::ParaConfig::isOptimizerListValid()) {
        optimizer::TerminalMessage::error(optimizer::ParaConfig::getOptimizerListError());
        return 1;
    }
    ensureResultDir();
    if (optimizer::ParaConfig::isTraceEnabled() || optimizer::ParaConfig::isDebugEnabled())
        ensureLogDir();
    static std::ofstream s_debugLog;
    if (optimizer::ParaConfig::isDebugEnabled()) {
        if (optimizer::openLogWithRotation("log/debug.log", s_debugLog, optimizer::ParaConfig::getDebugLogMaxBytes()))
            optimizer::ParaConfig::setDebugStream(&s_debugLog);
    }

    std::vector<optimizer::CoilEntry> coils;
    const std::string xcoilPath = optimizer::DataConfig::getXcoilFilePath();
    if (!xcoilPath.empty()) {
        int n = optimizer::loadCoilListFromFile(xcoilPath, coils);
        if (n >= 0) {
            const std::string dataPath = optimizer::DataConfig::getDataPath();
            const std::string structName = optimizer::DataConfig::getBinaryStructName();
            int existsCount = 0;
            for (const auto& e : coils) {
                std::string path = optimizer::CoilDataPath::buildPath(dataPath, e.coil_no, e.yyyymmdd, structName);
                if (optimizer::CoilDataPath::fileExists(path)) ++existsCount;
            }
            std::vector<std::string> lines = {
                "xcoil_file: " + xcoilPath,
                "data_path: " + dataPath,
                "binary_struct: " + structName,
                "コイル件数: " + std::to_string(coils.size()),
                "バイナリ存在: " + std::to_string(existsCount) + " / " + std::to_string(coils.size())
            };
            optimizer::TerminalMessage::summary("コイルデータ読込概要", lines);
            /* USERWORK: ここで全コイルを 1 度読んでそれぞれの製品情報を出力する処理をオンサイトで追加する。 */
        } else {
            optimizer::TerminalMessage::error("xcoil ファイルを開けません: " + xcoilPath);
        }
    }

    optimizer::ParameterMapper mapper;
    mapper.setSpecs(makeSpecs());

    std::vector<SummaryRow> summary;
    const std::vector<optimizer::ProductMeta> products = {{"demo1", ""}};

    optimizer::DemoPhysicalModel model1;
    optimizer::DemoDataLoader loader1;
    optimizer::DemoPhysicalModel2 model2;
    optimizer::DemoDataLoader2 loader2;
    optimizer::DemoPhysicalModel3 model3;
    optimizer::DemoDataLoader3 loader3;

    struct ModelEntry {
        int id;
        std::string name;
        optimizer::IPhysicalModel* model;
        optimizer::IProductDataLoader* loader;
    };
    std::vector<ModelEntry> models = {
        {1, "quadratic", &model1, &loader1},
        {2, "linear", &model2, &loader2},
        {3, "rational_exp", &model3, &loader3}
    };

    for (const auto& m : models) {
        for (const std::string& opt : handler.getOptimizersToRun()) {
            std::string tracePath = "log/model" + std::to_string(m.id) + "_" + opt + "_trace.csv";
            std::string logLabel = m.name + "-" + opt;
            optimizer::RunResult result = optimizer::OptimizerDriver::run(
                "config/para.cfg", mapper, *m.model, *m.loader, products, opt,
                tracePath, logLabel.c_str());
            SummaryRow r;
            r.model = m.name;
            r.optimizer = opt;
            r.final_rmse = result.bestScore;
            r.p0 = result.bestParams.size() > 0 ? result.bestParams[0] : 0;
            r.p1 = result.bestParams.size() > 1 ? result.bestParams[1] : 0;
            r.p2 = result.bestParams.size() > 2 ? result.bestParams[2] : 0;
            r.n_iter = nIterFor(opt);
            summary.push_back(r);
        }
    }

    std::ofstream sumFile("result/summary.csv");
    sumFile << "model,optimizer,final_rmse,p0,p1,p2,n_iter\n";
    for (const auto& r : summary)
        sumFile << r.model << "," << r.optimizer << "," << r.final_rmse << ","
                << r.p0 << "," << r.p1 << "," << r.p2 << "," << r.n_iter << "\n";

    std::cout << "[Demo] " << summary.size() << " runs done. result/summary.csv written.";
    if (optimizer::ParaConfig::isTraceEnabled())
        std::cout << " Trace logs in log/.";
    std::cout << "\n";
    return 0;
}
