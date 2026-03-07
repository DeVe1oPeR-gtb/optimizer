#ifndef OPTIMIZER_PARAM_PARAM_SPEC_H
#define OPTIMIZER_PARAM_PARAM_SPEC_H

#include <string>

namespace optimizer {

/// Initial value source for a parameter.
enum class InitMode {
    Manual,   /// use init_value only
    Db,       /// use DB value (db_key)
    Hybrid    /// use DB if available, else init_value
};

/// One row of parameter config (one parameter).
struct ParamSpec {
    std::string param_name;
    int enable_opt = 0;   /// 1 = include in optimization vector
    InitMode init_mode = InitMode::Manual;
    double init_value = 0.0;
    std::string db_key;
    double lower = 0.0;
    double upper = 0.0;
    std::string note;

    bool hasBounds() const {
        return enable_opt != 0;  // when enabled, lower/upper must be set (validated elsewhere)
    }
};

/// Parse init_mode string to enum. Returns InitMode::Manual for unknown.
InitMode parseInitMode(const std::string& s);

}  // namespace optimizer

#endif  // OPTIMIZER_PARAM_PARAM_SPEC_H
