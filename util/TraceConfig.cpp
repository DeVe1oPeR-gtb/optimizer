/**
 * @file TraceConfig.cpp
 * @brief 開発者用設定の読込（trace=on/off, optimizer=..., lm_apply_bounds=on/off）。key=val 形式。
 */

#include "util/TraceConfig.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace optimizer {

bool TraceConfig::traceEnabled_ = false;
std::ostream* TraceConfig::traceStream_ = nullptr;
std::vector<std::string> TraceConfig::optimizersToRun_ = {"PSO", "DE", "LM"};
bool TraceConfig::lmApplyBoundsEnabled_ = true;

void TraceConfig::loadFromStruct(const RunConfig& config) {
    traceEnabled_ = config.trace_enabled;
    optimizersToRun_ = config.optimizer_names.empty()
        ? std::vector<std::string>{"PSO", "DE", "LM"}
        : config.optimizer_names;
    lmApplyBoundsEnabled_ = config.lm_apply_bounds;
}

void TraceConfig::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return;
    std::string line;
    while (std::getline(f, line)) {
        auto sharp = line.find('#');
        if (sharp != std::string::npos) line = line.substr(0, sharp);
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key, val;
        for (size_t i = 0; i < eq; ++i)
            if (!std::isspace(static_cast<unsigned char>(line[i]))) key += line[i];
        for (size_t i = eq + 1; i < line.size(); ++i)
            if (!std::isspace(static_cast<unsigned char>(line[i]))) val += line[i];
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
        }
    }
}

bool TraceConfig::isTraceEnabled() { return traceEnabled_; }
bool TraceConfig::isLmApplyBoundsEnabled() { return lmApplyBoundsEnabled_; }
const std::vector<std::string>& TraceConfig::getOptimizersToRun() { return optimizersToRun_; }
std::ostream* TraceConfig::getTraceStream() { return traceStream_; }
void TraceConfig::setTraceStream(std::ostream* s) { traceStream_ = s; }

}  // namespace optimizer
