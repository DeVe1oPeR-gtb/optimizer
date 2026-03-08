/**
 * @file CoilList.cpp
 * @brief xcoil.txt から年月日・コイルNO のリストを読み込む。1 行 1 件、カンマまたは空白区切り。
 */

#include "util/CoilList.h"
#include <fstream>
#include <sstream>
#include <cctype>

namespace optimizer {

static std::string trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end == std::string::npos ? std::string::npos : end - start + 1);
}

int loadCoilListFromFile(const std::string& path, std::vector<CoilEntry>& out) {
    std::ifstream f(path);
    if (!f) return -1;
    int count = 0;
    std::string line;
    while (std::getline(f, line)) {
        auto sharp = line.find('#');
        if (sharp != std::string::npos) line = line.substr(0, sharp);
        line = trim(line);
        if (line.empty()) continue;
        std::string a, b;
        auto comma = line.find(',');
        if (comma != std::string::npos) {
            a = trim(line.substr(0, comma));
            b = trim(line.substr(comma + 1));
        } else {
            std::istringstream ss(line);
            if (ss >> a >> b) { /* ok */ }
        }
        if (a.empty() || b.empty()) continue;
        CoilEntry e;
        e.yyyymmdd = a;
        e.coil_no = b;
        out.push_back(e);
        ++count;
    }
    return count;
}

}  // namespace optimizer
