/**
 * @file CsvParamLoader.cpp
 * @brief CSV から ParamSpec 列を読み込み、検証する。列: param_name, enable_opt, init_mode, init_value, db_key, lower, upper, apply_bounds, note
 */

#include "CsvParamLoader.h"
#include <fstream>
#include <sstream>
#include <set>
#include <map>

namespace optimizer {

namespace {
const char kSep = ',';
}

std::string CsvParamLoader::trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end == std::string::npos ? std::string::npos : end - start + 1);
}

bool CsvParamLoader::parseRow(const std::string& line, std::vector<std::string>& cells) {
    cells.clear();
    std::string cell;
    bool inQuote = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inQuote) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    cell += '"';
                    ++i;
                } else {
                    inQuote = false;
                }
            } else {
                cell += c;
            }
        } else {
            if (c == '"') {
                inQuote = true;
            } else if (c == kSep) {
                cells.push_back(trim(cell));
                cell.clear();
            } else {
                cell += c;
            }
        }
    }
    cells.push_back(trim(cell));
    return true;
}

std::vector<ParamSpec> CsvParamLoader::load(const std::string& path, std::string& errorMessage) {
    errorMessage.clear();
    std::vector<ParamSpec> out;
    std::ifstream f(path);
    if (!f) {
        errorMessage = "Cannot open file: " + path;
        return out;
    }
    std::string line;
    if (!std::getline(f, line)) {
        errorMessage = "Empty file: " + path;
        return out;
    }
    std::vector<std::string> header;
    parseRow(line, header);
    auto col = [&header](const std::vector<std::string>& row, const std::string& name) -> std::string {
        for (size_t i = 0; i < header.size(); ++i) {
            if (trim(header[i]) == name && i < row.size())
                return trim(row[i]);
        }
        return "";
    };
    int rowNum = 1;
    while (std::getline(f, line)) {
        ++rowNum;
        if (trim(line).empty()) continue;
        std::vector<std::string> row;
        parseRow(line, row);
        ParamSpec spec;
        spec.param_name = col(row, "param_name");
        if (spec.param_name.empty()) {
            errorMessage = "Row " + std::to_string(rowNum) + ": param_name is empty";
            return {};
        }
        std::string eo = col(row, "enable_opt");
        spec.enable_opt = (eo == "1" || eo == "true" || eo == "yes") ? 1 : 0;
        spec.init_mode = parseInitMode(col(row, "init_mode"));
        try {
            std::string iv = col(row, "init_value");
            if (!iv.empty()) spec.init_value = std::stod(iv);
        } catch (...) {
            spec.init_value = 0.0;
        }
        spec.db_key = col(row, "db_key");
        try {
            std::string lo = col(row, "lower");
            if (!lo.empty()) spec.lower = std::stod(lo);
            std::string up = col(row, "upper");
            if (!up.empty()) spec.upper = std::stod(up);
        } catch (...) {}
        std::string ab = col(row, "apply_bounds");
        spec.apply_bounds = (ab.empty() || ab == "1" || ab == "on" || ab == "true" || ab == "yes");
        if (ab == "0" || ab == "off" || ab == "false" || ab == "no") spec.apply_bounds = false;
        spec.note = col(row, "note");
        out.push_back(spec);
    }
    return out;
}

bool CsvParamLoader::validate(const std::vector<ParamSpec>& specs,
                              const std::vector<std::string>& allowedParamNames,
                              std::string& errorMessage) const {
    errorMessage.clear();
    std::set<std::string> allowed(allowedParamNames.begin(), allowedParamNames.end());
    std::set<std::string> seen;
    for (const auto& s : specs) {
        if (s.param_name.empty()) {
            errorMessage = "param_name must not be empty";
            return false;
        }
        if (!allowed.empty() && allowed.count(s.param_name) == 0) {
            errorMessage = "Unknown parameter name: " + s.param_name;
            return false;
        }
        if (seen.count(s.param_name)) {
            errorMessage = "Duplicate parameter name: " + s.param_name;
            return false;
        }
        seen.insert(s.param_name);
        if (s.enable_opt != 0) {
            if (s.lower > s.upper) {
                errorMessage = "lower must be <= upper for " + s.param_name;
                return false;
            }
        }
    }
    for (const auto& s : specs) {
        if (s.enable_opt != 0) {
            double init = s.init_value;
            if (s.init_mode == InitMode::Db || s.init_mode == InitMode::Hybrid) {
                /* USERWORK: 本番ではここで DB 値を注入。検証時は DB 未使用のため init_value でチェック。 */
            }
            if (init < s.lower || init > s.upper) {
                errorMessage = "Initial value out of bounds for " + s.param_name;
                return false;
            }
        }
    }
    return true;
}

}  // namespace optimizer
