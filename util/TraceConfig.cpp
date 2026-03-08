/**
 * @file TraceConfig.cpp
 * @brief config/para.cfg の読込（trace, optimizer, 各最適化器パラメータ）。key=val 形式。
 */

#include "util/TraceConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace optimizer {

bool TraceConfig::traceEnabled_ = false;
std::ostream* TraceConfig::traceStream_ = nullptr;
std::vector<std::string> TraceConfig::optimizersToRun_ = {"PSO", "DE", "LM"};
bool TraceConfig::lmApplyBoundsEnabled_ = true;
int TraceConfig::nIterPso_ = 120;
int TraceConfig::nIterDe_ = 120;
int TraceConfig::nIterLm_ = 80;
double TraceConfig::psoW_ = 0.7;
double TraceConfig::psoC1_ = 1.8;
double TraceConfig::psoC2_ = 1.8;
int TraceConfig::psoNParticle_ = 20;
double TraceConfig::psoInitRadius_ = 0.5;
double TraceConfig::deF_ = 0.5;
double TraceConfig::deCr_ = 0.9;
int TraceConfig::deNPop_ = 20;
double TraceConfig::deInitRadius_ = 0.5;
double TraceConfig::lmLambdaInit_ = 10.0;
double TraceConfig::lmRPerturb_ = 0.005;
double TraceConfig::lmLambdaMin_ = 1e-10;
double TraceConfig::lmLambdaMax_ = 1e12;
double TraceConfig::lmLambdaDown_ = 0.5;
double TraceConfig::lmLambdaUp_ = 10.0;
int TraceConfig::lmMaxTry_ = 8;

void TraceConfig::loadFromStruct(const RunConfig& config) {
    traceEnabled_ = config.trace_enabled;
    optimizersToRun_ = config.optimizer_names.empty()
        ? std::vector<std::string>{"PSO", "DE", "LM"}
        : config.optimizer_names;
    lmApplyBoundsEnabled_ = config.lm_apply_bounds;
    nIterPso_ = config.n_iter_pso;
    nIterDe_ = config.n_iter_de;
    nIterLm_ = config.n_iter_lm;
}

static int parsePositiveInt(const std::string& val, int defaultVal) {
    const char* p = val.c_str();
    while (*p == ' ' || *p == '\t') ++p;
    if (*p == '\0') return defaultVal;
    char* end = nullptr;
    long n = std::strtol(p, &end, 10);
    if (end == p || n <= 0 || n > 1000000) return defaultVal;
    return static_cast<int>(n);
}

static double parseDouble(const std::string& val, double defaultVal) {
    const char* p = val.c_str();
    while (*p == ' ' || *p == '\t') ++p;
    if (*p == '\0') return defaultVal;
    char* end = nullptr;
    double x = std::strtod(p, &end);
    if (end == p) return defaultVal;
    return x;
}

void TraceConfig::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return;
    std::string line;
    while (std::getline(f, line)) {
        auto sharp = line.find('#');
        if (sharp != std::string::npos) line = line.substr(0, sharp);
        std::istringstream lineStream(line);
        std::string segment;
        while (std::getline(lineStream, segment, ';')) {
            auto sharpSeg = segment.find('#');
            if (sharpSeg != std::string::npos) segment = segment.substr(0, sharpSeg);
            auto eq = segment.find('=');
            if (eq == std::string::npos) continue;
            std::string key, val;
            for (size_t i = 0; i < eq; ++i)
                if (!std::isspace(static_cast<unsigned char>(segment[i]))) key += segment[i];
            for (size_t i = eq + 1; i < segment.size(); ++i)
                if (!std::isspace(static_cast<unsigned char>(segment[i]))) val += segment[i];
            if (key == "trace") {
            std::string v;
            for (char c : val) v += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            traceEnabled_ = (v == "on" || v == "1" || v == "true" || v == "yes");
        } else if (key == "lm_apply_bounds") {
            std::string v;
            for (char c : val) v += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            lmApplyBoundsEnabled_ = (v == "on" || v == "1" || v == "true" || v == "yes");
        } else if (key == "optimizer") {
            optimizersToRun_.clear();
            std::istringstream ss(val);
            std::string name;
            while (std::getline(ss, name, ',')) {
                name.erase(0, name.find_first_not_of(" \t"));
                name.erase(name.find_last_not_of(" \t") + 1);
                if (!name.empty()) optimizersToRun_.push_back(name);
            }
            if (optimizersToRun_.empty()) optimizersToRun_ = {"PSO", "DE", "LM"};
        } else if (key == "n_iter_pso") {
            nIterPso_ = parsePositiveInt(val, nIterPso_);
        } else if (key == "n_iter_de") {
            nIterDe_ = parsePositiveInt(val, nIterDe_);
        } else if (key == "n_iter_lm") {
            nIterLm_ = parsePositiveInt(val, nIterLm_);
        } else if (key == "pso_w") {
            psoW_ = parseDouble(val, psoW_);
        } else if (key == "pso_c1") {
            psoC1_ = parseDouble(val, psoC1_);
        } else if (key == "pso_c2") {
            psoC2_ = parseDouble(val, psoC2_);
        } else if (key == "pso_n_particle") {
            psoNParticle_ = parsePositiveInt(val, psoNParticle_);
        } else if (key == "pso_init_radius") {
            psoInitRadius_ = parseDouble(val, psoInitRadius_);
        } else if (key == "de_f") {
            deF_ = parseDouble(val, deF_);
        } else if (key == "de_cr") {
            deCr_ = parseDouble(val, deCr_);
        } else if (key == "de_n_pop") {
            deNPop_ = parsePositiveInt(val, deNPop_);
        } else if (key == "de_init_radius") {
            deInitRadius_ = parseDouble(val, deInitRadius_);
        } else if (key == "lm_lambda_init") {
            lmLambdaInit_ = parseDouble(val, lmLambdaInit_);
        } else if (key == "lm_r_perturb") {
            lmRPerturb_ = parseDouble(val, lmRPerturb_);
        } else if (key == "lm_lambda_min") {
            lmLambdaMin_ = parseDouble(val, lmLambdaMin_);
        } else if (key == "lm_lambda_max") {
            lmLambdaMax_ = parseDouble(val, lmLambdaMax_);
        } else if (key == "lm_lambda_down") {
            lmLambdaDown_ = parseDouble(val, lmLambdaDown_);
        } else if (key == "lm_lambda_up") {
            lmLambdaUp_ = parseDouble(val, lmLambdaUp_);
        } else if (key == "lm_max_try") {
            int v = parsePositiveInt(val, lmMaxTry_);
            lmMaxTry_ = (v >= 1) ? v : 1;
        }
        }
    }
}

bool TraceConfig::isTraceEnabled() { return traceEnabled_; }
bool TraceConfig::isLmApplyBoundsEnabled() { return lmApplyBoundsEnabled_; }
const std::vector<std::string>& TraceConfig::getOptimizersToRun() { return optimizersToRun_; }
std::ostream* TraceConfig::getTraceStream() { return traceStream_; }
void TraceConfig::setTraceStream(std::ostream* s) { traceStream_ = s; }
int TraceConfig::getNIterPso() { return nIterPso_; }
int TraceConfig::getNIterDe() { return nIterDe_; }
int TraceConfig::getNIterLm() { return nIterLm_; }
double TraceConfig::getPsoW() { return psoW_; }
double TraceConfig::getPsoC1() { return psoC1_; }
double TraceConfig::getPsoC2() { return psoC2_; }
int TraceConfig::getPsoNParticle() { return psoNParticle_; }
double TraceConfig::getPsoInitRadius() { return psoInitRadius_; }
double TraceConfig::getDeF() { return deF_; }
double TraceConfig::getDeCr() { return deCr_; }
int TraceConfig::getDeNPop() { return deNPop_; }
double TraceConfig::getDeInitRadius() { return deInitRadius_; }
double TraceConfig::getLmLambdaInit() { return lmLambdaInit_; }
double TraceConfig::getLmRPerturb() { return lmRPerturb_; }
double TraceConfig::getLmLambdaMin() { return lmLambdaMin_; }
double TraceConfig::getLmLambdaMax() { return lmLambdaMax_; }
double TraceConfig::getLmLambdaDown() { return lmLambdaDown_; }
double TraceConfig::getLmLambdaUp() { return lmLambdaUp_; }
int TraceConfig::getLmMaxTry() { return lmMaxTry_; }

}  // namespace optimizer
