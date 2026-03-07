#include "util/TraceConfig.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace optimizer {

bool TraceConfig::traceEnabled_ = false;
std::ostream* TraceConfig::traceStream_ = nullptr;

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
            break;
        }
    }
}

bool TraceConfig::isTraceEnabled() { return traceEnabled_; }
std::ostream* TraceConfig::getTraceStream() { return traceStream_; }
void TraceConfig::setTraceStream(std::ostream* s) { traceStream_ = s; }

}  // namespace optimizer
