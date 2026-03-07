/**
 * @file DataConfig.cpp
 * @brief コイルデータ用設定の読込（xcoil_file, data_path, binary_struct）。developer.cfg の key=val を解釈。
 */

#include "util/DataConfig.h"
#include <fstream>
#include <cctype>

namespace optimizer {

std::string DataConfig::xcoilFilePath_;
std::string DataConfig::dataPath_;
std::string DataConfig::binaryStructName_;

void DataConfig::load(const std::string& path) {
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
        if (key == "xcoil_file") {
            xcoilFilePath_ = val;
        } else if (key == "data_path") {
            dataPath_ = val;
        } else if (key == "binary_struct") {
            binaryStructName_ = val;
        }
    }
}

const std::string& DataConfig::getXcoilFilePath() { return xcoilFilePath_; }
const std::string& DataConfig::getDataPath() { return dataPath_; }
const std::string& DataConfig::getBinaryStructName() { return binaryStructName_; }

}  // namespace optimizer
