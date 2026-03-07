#ifndef OPTIMIZER_PARAM_PARAMETER_MAPPER_H
#define OPTIMIZER_PARAM_PARAMETER_MAPPER_H

#include "ParamSpec.h"
#include <vector>
#include <string>

namespace optimizer {

/// USERWORK: 現場DBファイルから db_key に対応する値を取得する実装に差し替える。
/// In production, DB values are loaded from real DB. This interface allows injecting values for db_key.
using DbValueProvider = double (*)(const std::string& db_key);

/// Maps between full parameter set (all params for the physical model) and
/// optimization vector (only enable_opt parameters). Order of full params = order of CSV rows.
class ParameterMapper {
public:
    ParameterMapper() = default;

    /// Build from validated specs (order of specs = order of full parameter vector).
    void setSpecs(std::vector<ParamSpec> specs);

    /// Optional: set allowed param names for validate(). Empty = allow any.
    void setAllowedParamNames(std::vector<std::string> names);
    bool validate(std::string& errorMessage) const;

    /// Number of parameters in optimization vector.
    size_t numOptParams() const;
    /// Total number of parameters (full set).
    size_t numFullParams() const;

    /// Initial vector for optimization (based on init_mode: manual / db / hybrid).
    /// USERWORK: For db/hybrid, provide dbValueProvider; otherwise init_value is used.
    std::vector<double> getInitialVector(DbValueProvider dbValueProvider = nullptr) const;

    /// Bounds for optimization vector only (same order as getInitialVector).
    std::vector<double> getLowerBounds() const;
    std::vector<double> getUpperBounds() const;

    /// Expand x_opt into full parameter set (same order as specs).
    /// USERWORK: FullParameterSet layout (which index is which physical param) is product-specific.
    std::vector<double> expandToFullParameterSet(const std::vector<double>& x_opt) const;

    /// Extract optimization vector from full parameter set.
    std::vector<double> fullToOptVector(const std::vector<double>& full) const;

    const std::vector<ParamSpec>& specs() const { return specs_; }

private:
    std::vector<ParamSpec> specs_;
    std::vector<std::string> allowedParamNames_;
    std::vector<size_t> optIndices_;  // indices into full for each opt dimension
};

}  // namespace optimizer

#endif  // OPTIMIZER_PARAM_PARAMETER_MAPPER_H
