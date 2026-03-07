#ifndef OPTIMIZER_PARAM_CSV_PARAM_LOADER_H
#define OPTIMIZER_PARAM_CSV_PARAM_LOADER_H

#include "ParamSpec.h"
#include <string>
#include <vector>

namespace optimizer {

/// Load and validate parameter config from CSV.
/// CSV: one row per parameter, columns param_name, enable_opt, init_mode, init_value, db_key, lower, upper, note.
class CsvParamLoader {
public:
    /// Load specs from file. Returns empty vector and sets errorMessage on failure.
    std::vector<ParamSpec> load(const std::string& path, std::string& errorMessage);

    /// Validate specs. Returns true if valid; otherwise errorMessage is set.
    bool validate(const std::vector<ParamSpec>& specs,
                  const std::vector<std::string>& allowedParamNames,
                  std::string& errorMessage) const;

private:
    static std::string trim(const std::string& s);
    static bool parseRow(const std::string& line, std::vector<std::string>& cells);
};

}  // namespace optimizer

#endif  // OPTIMIZER_PARAM_CSV_PARAM_LOADER_H
