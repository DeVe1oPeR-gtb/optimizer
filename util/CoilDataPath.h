#ifndef OPTIMIZER_UTIL_COIL_DATA_PATH_H
#define OPTIMIZER_UTIL_COIL_DATA_PATH_H

#include <string>

namespace optimizer {

/**
 * @brief コイルバイナリファイルのパス規則と存在チェック
 *
 * ファイル名規則: コイルNO(6桁)_年月日(yyyymmdd).構造体名
 * 例: 000123_20250107.MyStruct
 * コイルNO は 6 桁にゼロパディングする。
 */
namespace CoilDataPath {

/**
 * @brief バイナリファイルのフルパスを組み立てる
 * @param dataPath バイナリのベースディレクトリ（末尾の / はあってもなくてもよい）
 * @param coilNo コイルNO（数字のみ想定。6桁未満はゼロパディング）
 * @param yyyymmdd 年月日 (yyyymmdd)
 * @param binaryStructName 拡張子に使う構造体名（ドットは含めない）
 */
std::string buildPath(const std::string& dataPath,
                      const std::string& coilNo,
                      const std::string& yyyymmdd,
                      const std::string& binaryStructName);

/** @brief ファイルが存在するか */
bool fileExists(const std::string& path);

}  // namespace CoilDataPath

}  // namespace optimizer

#endif
