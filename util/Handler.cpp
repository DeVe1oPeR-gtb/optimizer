#include "util/Handler.h"
#include "util/TraceConfig.h"

Handler::Handler(const std::string& configPath) {
    optimizer::TraceConfig::load(configPath);
}

Handler::~Handler() {}

std::vector<std::string> Handler::getOptimizersToRun() const {
    return optimizer::TraceConfig::getOptimizersToRun();
}
