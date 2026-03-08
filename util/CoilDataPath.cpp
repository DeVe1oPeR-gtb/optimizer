/**
 * @file CoilDataPath.cpp
 * @brief コイルバイナリのファイルパス組み立て（コイルNO 6 桁_zero-pad_yyyymmdd.拡張子）と存在チェック。
 */

#include "util/util_common.hpp"
#include <fstream>
#include <sstream>
#include <cctype>

namespace optimizer {

namespace CoilDataPath {

std::string buildPath(const std::string& dataPath,
                      const std::string& coilNo,
                      const std::string& yyyymmdd,
                      const std::string& binaryStructName) {
    // 空の場合は不正なパスを返さない。呼び出し側が無効パスで open してしまうのを防ぐため。
    if (dataPath.empty() || binaryStructName.empty()) return "";
    std::string no = coilNo;
    // コイルNO は現場のファイル名規則で常に6桁（ゼロパディング）のため、不足分を先頭に 0 で埋める。
    while (no.size() < 6u) no = "0" + no;
    // 6桁を超える場合は末尾6桁のみ使う。既存ファイル名（NNNNNN_yyyymmdd.ext）との整合を取るため。
    if (no.size() > 6u) no = no.substr(no.size() - 6);
    std::string base = dataPath;
    // dataPath に末尾スラッシュが無いときは区切りを補う。OS ごとの区切り文字（/ or \）は呼び出し側の設定に任せる。
    if (!base.empty() && base.back() != '/' && base.back() != '\\')
        base += '/';
    return base + no + "_" + yyyymmdd + "." + binaryStructName;
}

bool fileExists(const std::string& path) {
    if (path.empty()) return false;
    // ディレクトリではなく「ファイルとして開けるか」を見たいため ifstream で開く。stat より移植性を優先。
    std::ifstream f(path);
    return f.good();
}

}  // namespace CoilDataPath

}  // namespace optimizer
