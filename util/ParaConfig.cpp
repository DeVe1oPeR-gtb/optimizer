/**
 * @file ParaConfig.cpp
 * @brief config/para.cfg の読込（trace, optimizer, 各最適化器パラメータ）。key=val 形式。
 */

#include "util/ParaConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>

namespace optimizer {

bool ParaConfig::traceEnabled_ = false;
bool ParaConfig::debugEnabled_ = false;
std::ostream* ParaConfig::traceStream_ = nullptr;
std::ostream* ParaConfig::debugStream_ = nullptr;
std::vector<std::string> ParaConfig::optimizersToRun_;
bool ParaConfig::optimizerListValid_ = false;
std::string ParaConfig::optimizerListError_ = "optimizer= が指定されていません。";
bool ParaConfig::lmApplyBoundsEnabled_ = true;
int ParaConfig::nIterPso_ = 120;
int ParaConfig::nIterDe_ = 120;
int ParaConfig::nIterLm_ = 80;
double ParaConfig::psoW_ = 0.7;
double ParaConfig::psoC1_ = 1.8;
double ParaConfig::psoC2_ = 1.8;
int ParaConfig::psoNParticle_ = 20;
double ParaConfig::psoInitRadius_ = 0.5;
double ParaConfig::deF_ = 0.5;
double ParaConfig::deCr_ = 0.9;
int ParaConfig::deNPop_ = 20;
double ParaConfig::deInitRadius_ = 0.5;
double ParaConfig::lmLambdaInit_ = 10.0;
double ParaConfig::lmRPerturb_ = 0.005;
double ParaConfig::lmLambdaMin_ = 1e-10;
double ParaConfig::lmLambdaMax_ = 1e12;
double ParaConfig::lmLambdaDown_ = 0.5;
double ParaConfig::lmLambdaUp_ = 10.0;
int ParaConfig::lmMaxTry_ = 8;
size_t ParaConfig::traceLogMaxBytes_ = 10 * 1024 * 1024;   // 10 MiB
size_t ParaConfig::debugLogMaxBytes_ = 1 * 1024 * 1024;   // 1 MiB
bool ParaConfig::plogEnabled_ = true;
bool ParaConfig::llogEnabled_ = false;
bool ParaConfig::dlogEnabled_ = false;
std::string ParaConfig::plogFilename_;
std::string ParaConfig::csvFilenameAfter_;
bool ParaConfig::detailEnabled_ = false;
int ParaConfig::detailStartIndex_ = 0;
int ParaConfig::detailMaxPoints_ = 0;
bool ParaConfig::llogOneFile_ = true;
std::string ParaConfig::llogFilename_;
std::string ParaConfig::dlogFilename_;
size_t ParaConfig::resultFileMaxBytes_ = 0;
size_t ParaConfig::resultTotalMaxBytes_ = 0;
std::string ParaConfig::resultFinalParamsFilename_;
std::vector<std::string> ParaConfig::optimizationDataTypes_;
double ParaConfig::optimizationPositionMin_ = 0.0;
double ParaConfig::optimizationPositionMax_ = 1.0;

void ParaConfig::loadFromStruct(const RunConfig& config) {
    traceEnabled_ = config.trace_enabled;
    /* debug は loadFromStruct では変更しない（cfg の debug= のみ） */
    if (config.optimizer_names.empty()) {
        optimizersToRun_.clear();
        optimizerListValid_ = false;
        optimizerListError_ = "実行する最適化器を1つ以上指定してください。";
    } else {
        optimizersToRun_.clear();
        optimizerListValid_ = true;
        optimizerListError_.clear();
        for (const std::string& name : config.optimizer_names) {
            if (name != "PSO" && name != "DE" && name != "LM") {
                optimizerListValid_ = false;
                optimizerListError_ = "不正な最適化器名: " + name + "。PSO, DE, LM のいずれかを指定してください。";
                optimizersToRun_.clear();
                break;
            }
            optimizersToRun_.push_back(name);
        }
    }
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

static int parseNonNegativeInt(const std::string& val, int defaultVal) {
    const char* p = val.c_str();
    while (*p == ' ' || *p == '\t') ++p;
    if (*p == '\0') return defaultVal;
    char* end = nullptr;
    long n = std::strtol(p, &end, 10);
    if (end == p || n < 0 || n > 1000000) return defaultVal;
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

static long parsePositiveLong(const std::string& val, long defaultVal) {
    const char* p = val.c_str();
    while (*p == ' ' || *p == '\t') ++p;
    if (*p == '\0') return defaultVal;
    char* end = nullptr;
    long n = std::strtol(p, &end, 10);
    if (end == p || n < 0) return defaultVal;
    return n;
}

static bool isValidOptimizerName(const std::string& s) {
    return s == "PSO" || s == "DE" || s == "LM" || s == "INIT" || s == "DB";
}

void ParaConfig::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return;
    bool optimizerKeySeen = false;
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
        } else if (key == "debug") {
            std::string v;
            for (char c : val) v += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            debugEnabled_ = (v == "on" || v == "1" || v == "true" || v == "yes");
        } else if (key == "lm_apply_bounds") {
            std::string v;
            for (char c : val) v += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            lmApplyBoundsEnabled_ = (v == "on" || v == "1" || v == "true" || v == "yes");
        } else if (key == "optimizer") {
            optimizerKeySeen = true;
            optimizersToRun_.clear();
            optimizerListValid_ = true;
            optimizerListError_.clear();
            std::string valTrim = val;
            valTrim.erase(0, valTrim.find_first_not_of(" \t"));
            valTrim.erase(valTrim.find_last_not_of(" \t") + 1);
            if (valTrim == "ALL_OPT_EXEC") {
                optimizersToRun_ = {"PSO", "DE", "LM"};
            } else if (valTrim == "INIT") {
                optimizersToRun_ = {"INIT"};
            } else if (valTrim == "DB") {
                optimizersToRun_ = {"DB"};
            } else {
                std::istringstream ss(val);
                std::string name;
                while (std::getline(ss, name, ',')) {
                    name.erase(0, name.find_first_not_of(" \t"));
                    name.erase(name.find_last_not_of(" \t") + 1);
                    if (name.empty()) continue;
                    if (!isValidOptimizerName(name)) {
                        optimizerListValid_ = false;
                        optimizerListError_ = "不正な最適化器名: " + name + "。PSO, DE, LM, INIT, DB のいずれかを指定してください。";
                        optimizersToRun_.clear();
                        break;
                    }
                    optimizersToRun_.push_back(name);
                }
                if (optimizerListValid_ && optimizersToRun_.empty()) {
                    optimizerListValid_ = false;
                    optimizerListError_ = "実行する最適化器を1つ以上指定してください。";
                }
            }
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
        } else if (key == "trace_log_max_bytes") {
            long v = parsePositiveLong(val, static_cast<long>(traceLogMaxBytes_));
            traceLogMaxBytes_ = (v > 0) ? static_cast<size_t>(v) : traceLogMaxBytes_;
        } else if (key == "debug_log_max_bytes") {
            long v = parsePositiveLong(val, static_cast<long>(debugLogMaxBytes_));
            debugLogMaxBytes_ = (v > 0) ? static_cast<size_t>(v) : debugLogMaxBytes_;
        } else if (key == "plog_enabled") {
            plogEnabled_ = (val == "1" || val == "on" || val == "true");
        } else if (key == "llog_enabled") {
            llogEnabled_ = (val == "1" || val == "on" || val == "true");
        } else if (key == "dlog_enabled") {
            dlogEnabled_ = (val == "1" || val == "on" || val == "true");
        } else if (key == "plog_filename" || key == "result_filename_before") {
            plogFilename_ = val;
        } else if (key == "csv_filename_after" || key == "result_filename_after") {
            csvFilenameAfter_ = val;
        } else if (key == "detail_enabled" || key == "result_detail_enabled") {
            detailEnabled_ = (val == "1" || val == "on" || val == "true");
        } else if (key == "detail_start_index" || key == "result_detail_start_index") {
            detailStartIndex_ = parseNonNegativeInt(val, detailStartIndex_);
        } else if (key == "detail_max_points" || key == "result_detail_max_points") {
            detailMaxPoints_ = parseNonNegativeInt(val, detailMaxPoints_);
        } else if (key == "llog_one_file" || key == "result_detail_one_file") {
            llogOneFile_ = (val != "0" && val != "off" && val != "false");
        } else if (key == "llog_filename" || key == "result_detail_filename") {
            llogFilename_ = val;
        } else if (key == "dlog_filename" || key == "result_detail_filename_per_product") {
            dlogFilename_ = val;
        } else if (key == "result_file_max_bytes") {
            long v = parsePositiveLong(val, static_cast<long>(resultFileMaxBytes_));
            resultFileMaxBytes_ = (v >= 0) ? static_cast<size_t>(v) : resultFileMaxBytes_;
        } else if (key == "result_total_max_bytes") {
            long v = parsePositiveLong(val, static_cast<long>(resultTotalMaxBytes_));
            resultTotalMaxBytes_ = (v >= 0) ? static_cast<size_t>(v) : resultTotalMaxBytes_;
        } else if (key == "result_final_params_filename") {
            resultFinalParamsFilename_ = val;
        } else if (key == "optimization_data_types") {
            optimizationDataTypes_.clear();
            std::string v = val;
            size_t pos = 0;
            while (pos < v.size()) {
                size_t next = v.find(',', pos);
                if (next == std::string::npos) next = v.size();
                std::string part = v.substr(pos, next - pos);
                part.erase(0, part.find_first_not_of(" \t"));
                part.erase(part.find_last_not_of(" \t") + 1);
                if (!part.empty())
                    optimizationDataTypes_.push_back(part);
                pos = (next < v.size()) ? next + 1 : v.size();
            }
        } else if (key == "optimization_position_min") {
            optimizationPositionMin_ = parseDouble(val, optimizationPositionMin_);
        } else if (key == "optimization_position_max") {
            optimizationPositionMax_ = parseDouble(val, optimizationPositionMax_);
        }
        }
    }
    if (!optimizerKeySeen) {
        optimizersToRun_.clear();
        optimizerListValid_ = false;
        optimizerListError_ = "optimizer= が指定されていません。PSO, DE, LM, INIT, DB または ALL_OPT_EXEC を指定してください。";
    }
}

bool ParaConfig::isTraceEnabled() { return traceEnabled_; }
bool ParaConfig::isDebugEnabled() { return debugEnabled_; }
std::ostream* ParaConfig::getDebugStream() { return debugStream_; }
void ParaConfig::setDebugStream(std::ostream* s) { debugStream_ = s; }

void ParaConfig::logDebug(const std::string& message) {
    if (!debugEnabled_) return;
    std::ostream* out = debugStream_ ? debugStream_ : &std::cerr;
    *out << "[debug] " << message << "\n";
    if (out != &std::cerr) out->flush();
}

size_t ParaConfig::getTraceLogMaxBytes() { return traceLogMaxBytes_; }
size_t ParaConfig::getDebugLogMaxBytes() { return debugLogMaxBytes_; }

bool ParaConfig::isLmApplyBoundsEnabled() { return lmApplyBoundsEnabled_; }
const std::vector<std::string>& ParaConfig::getOptimizersToRun() { return optimizersToRun_; }
bool ParaConfig::isOptimizerListValid() { return optimizerListValid_; }
const std::string& ParaConfig::getOptimizerListError() { return optimizerListError_; }
std::ostream* ParaConfig::getTraceStream() { return traceStream_; }
void ParaConfig::setTraceStream(std::ostream* s) { traceStream_ = s; }
int ParaConfig::getNIterPso() { return nIterPso_; }
int ParaConfig::getNIterDe() { return nIterDe_; }
int ParaConfig::getNIterLm() { return nIterLm_; }
double ParaConfig::getPsoW() { return psoW_; }
double ParaConfig::getPsoC1() { return psoC1_; }
double ParaConfig::getPsoC2() { return psoC2_; }
int ParaConfig::getPsoNParticle() { return psoNParticle_; }
double ParaConfig::getPsoInitRadius() { return psoInitRadius_; }
double ParaConfig::getDeF() { return deF_; }
double ParaConfig::getDeCr() { return deCr_; }
int ParaConfig::getDeNPop() { return deNPop_; }
double ParaConfig::getDeInitRadius() { return deInitRadius_; }
double ParaConfig::getLmLambdaInit() { return lmLambdaInit_; }
double ParaConfig::getLmRPerturb() { return lmRPerturb_; }
double ParaConfig::getLmLambdaMin() { return lmLambdaMin_; }
double ParaConfig::getLmLambdaMax() { return lmLambdaMax_; }
double ParaConfig::getLmLambdaDown() { return lmLambdaDown_; }
double ParaConfig::getLmLambdaUp() { return lmLambdaUp_; }
int ParaConfig::getLmMaxTry() { return lmMaxTry_; }

bool ParaConfig::getPLOGEnabled() { return plogEnabled_; }
bool ParaConfig::getLLOGEnabled() { return llogEnabled_; }
bool ParaConfig::getDLOGEnabled() { return dlogEnabled_; }
const std::string& ParaConfig::getPLOGFilename() { return plogFilename_; }
const std::string& ParaConfig::getCsvFilenameAfter() { return csvFilenameAfter_; }
bool ParaConfig::getDetailEnabled() { return detailEnabled_; }
int ParaConfig::getDetailStartIndex() { return detailStartIndex_; }
int ParaConfig::getDetailMaxPoints() { return detailMaxPoints_; }
bool ParaConfig::getLLOGOneFile() { return llogOneFile_; }
const std::string& ParaConfig::getLLOGFilename() { return llogFilename_; }
const std::string& ParaConfig::getDLOGFilename() { return dlogFilename_; }
size_t ParaConfig::getResultFileMaxBytes() { return resultFileMaxBytes_; }
size_t ParaConfig::getResultTotalMaxBytes() { return resultTotalMaxBytes_; }
const std::string& ParaConfig::getResultFinalParamsFilename() { return resultFinalParamsFilename_; }

const std::vector<std::string>& ParaConfig::getOptimizationDataTypes() { return optimizationDataTypes_; }

bool ParaConfig::isDataTypeUsedForOptimization(const std::string& data_type_id) {
    if (optimizationDataTypes_.empty()) return true;
    for (const auto& t : optimizationDataTypes_)
        if (t == data_type_id) return true;
    return false;
}

double ParaConfig::getOptimizationPositionMin() { return optimizationPositionMin_; }
double ParaConfig::getOptimizationPositionMax() { return optimizationPositionMax_; }

void ParaConfig::resetForTest() {
    optimizersToRun_.clear();
    optimizersToRun_.shrink_to_fit();
    optimizerListValid_ = false;
    optimizerListError_ = "optimizer= が指定されていません。";
    plogFilename_.clear();
    plogFilename_.shrink_to_fit();
    csvFilenameAfter_.clear();
    csvFilenameAfter_.shrink_to_fit();
    llogFilename_.clear();
    llogFilename_.shrink_to_fit();
    dlogFilename_.clear();
    dlogFilename_.shrink_to_fit();
    resultFinalParamsFilename_.clear();
    resultFinalParamsFilename_.shrink_to_fit();
    optimizationDataTypes_.clear();
    optimizationDataTypes_.shrink_to_fit();
}

}  // namespace optimizer
