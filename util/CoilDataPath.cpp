/**
 * @file CoilDataPath.cpp
 * @brief コイルバイナリのファイルパス組み立て（コイルNO 6 桁_zero-pad_yyyymmdd.拡張子）と存在チェック。
 */

#include "util/CoilDataPath.hpp"
#include <fstream>
#include <sstream>
#include <cctype>

namespace optimizer {

namespace CoilDataPath {

std::string buildPath(const std::string& dataPath,
                      const std::string& coilNo,
                      const std::string& yyyymmdd,
                      const std::string& binaryStructName) {
    if (dataPath.empty() || binaryStructName.empty()) return "";
    std::string no = coilNo;
    while (no.size() < 6u) no = "0" + no;
    if (no.size() > 6u) no = no.substr(no.size() - 6);
    std::string base = dataPath;
    if (!base.empty() && base.back() != '/' && base.back() != '\\')
        base += '/';
    return base + no + "_" + yyyymmdd + "." + binaryStructName;
}

bool fileExists(const std::string& path) {
    if (path.empty()) return false;
    std::ifstream f(path);
    return f.good();
}

}  // namespace CoilDataPath

}  // namespace optimizer
