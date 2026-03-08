/**
 * @file ParameterMapper.cpp
 * @brief 最適化ベクトルと全パラメータの対応付け。初期値・上下限・apply_bounds は specs から取得。
 */

#include "param/param.hpp"
#include <stdexcept>

namespace optimizer {

void ParameterMapper::setSpecs(std::vector<ParamSpec> specs) {
    specs_ = std::move(specs);
    optIndices_.clear();
    for (size_t i = 0; i < specs_.size(); ++i) {
        if (specs_[i].enable_opt != 0)
            optIndices_.push_back(i);
    }
}

void ParameterMapper::setAllowedParamNames(std::vector<std::string> names) {
    allowedParamNames_ = std::move(names);
}

bool ParameterMapper::validate(std::string& errorMessage) const {
    CsvParamLoader loader;
    return loader.validate(specs_, allowedParamNames_, errorMessage);
}

size_t ParameterMapper::numOptParams() const {
    return optIndices_.size();
}

size_t ParameterMapper::numFullParams() const {
    return specs_.size();
}

std::vector<double> ParameterMapper::getInitialVector(DbValueProvider dbValueProvider) const {
    std::vector<double> x(numOptParams());
    for (size_t k = 0; k < optIndices_.size(); ++k) {
        size_t i = optIndices_[k];
        const auto& s = specs_[i];
        double v = s.init_value;
        if (s.init_mode == InitMode::Db && dbValueProvider && !s.db_key.empty()) {
            v = dbValueProvider(s.db_key);
        } else if (s.init_mode == InitMode::Hybrid && dbValueProvider && !s.db_key.empty()) {
            v = dbValueProvider(s.db_key);
            /* USERWORK: DB に値が無い場合は init_value にフォールバックする。ここでは未取得時は init_value を返す前提。 */
        }
        x[k] = v;
    }
    return x;
}

std::vector<double> ParameterMapper::getLowerBounds() const {
    std::vector<double> lo(numOptParams());
    for (size_t k = 0; k < optIndices_.size(); ++k)
        lo[k] = specs_[optIndices_[k]].lower;
    return lo;
}

std::vector<double> ParameterMapper::getUpperBounds() const {
    std::vector<double> up(numOptParams());
    for (size_t k = 0; k < optIndices_.size(); ++k)
        up[k] = specs_[optIndices_[k]].upper;
    return up;
}

std::vector<bool> ParameterMapper::getApplyBounds() const {
    std::vector<bool> out(numOptParams());
    for (size_t k = 0; k < optIndices_.size(); ++k)
        out[k] = specs_[optIndices_[k]].apply_bounds;
    return out;
}

std::vector<double> ParameterMapper::expandToFullParameterSet(const std::vector<double>& x_opt) const {
    if (x_opt.size() != numOptParams()) {
        throw std::invalid_argument("ParameterMapper::expandToFullParameterSet size mismatch");
    }
    std::vector<double> full(numFullParams());
    for (size_t i = 0; i < specs_.size(); ++i)
        full[i] = specs_[i].init_value;
    for (size_t k = 0; k < optIndices_.size(); ++k)
        full[optIndices_[k]] = x_opt[k];
    return full;
}

std::vector<double> ParameterMapper::fullToOptVector(const std::vector<double>& full) const {
    if (full.size() != numFullParams()) {
        throw std::invalid_argument("ParameterMapper::fullToOptVector size mismatch");
    }
    std::vector<double> x(numOptParams());
    for (size_t k = 0; k < optIndices_.size(); ++k)
        x[k] = full[optIndices_[k]];
    return x;
}

}  // namespace optimizer
