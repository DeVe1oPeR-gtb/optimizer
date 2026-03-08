/**
 * @file ParamSpec.cpp
 * @brief パラメータ設定のヘルパ（init_mode 文字列のパース）
 */

#include "ParamSpec.h"
#include <algorithm>
#include <cctype>

namespace optimizer {

InitMode parseInitMode(const std::string& s) {
    std::string t;
    for (char c : s) {
        t += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    if (t == "manual") return InitMode::Manual;
    if (t == "db") return InitMode::Db;
    if (t == "hybrid") return InitMode::Hybrid;
    return InitMode::Manual;
}

}  // namespace optimizer
