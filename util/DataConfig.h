#ifndef OPTIMIZER_UTIL_DATA_CONFIG_H
#define OPTIMIZER_UTIL_DATA_CONFIG_H

/**
 * @file DataConfig.h
 * @brief コイルデータ用設定（xcoil_file, data_path, binary_struct）を developer.cfg から読む。
 */

#include <string>

namespace optimizer {

/**
 * @brief コイルデータ用設定（developer.cfg から読込）
 *
 * xcoil_file: 年月日・コイルNO リストのファイルパス（例: xcoil.txt）
 * data_path:  バイナリファイルのベースディレクトリ
 * binary_struct: バイナリの拡張子に使う構造体名（ファイル名規則: コイルNO(6桁)_年月日(yyyymmdd).binary_struct）
 */
class DataConfig {
public:
    /** @brief 設定ファイルを読み込む（同じ key=val 形式）。未指定の項目は空のまま */
    static void load(const std::string& path);

    /** @brief 年月日・コイルNO リストファイルのパス（未設定時は空） */
    static const std::string& getXcoilFilePath();
    /** @brief バイナリデータのベースディレクトリ（未設定時は空） */
    static const std::string& getDataPath();
    /** @brief バイナリファイルの構造体名（拡張子。未設定時は空） */
    static const std::string& getBinaryStructName();

private:
    static std::string xcoilFilePath_;
    static std::string dataPath_;
    static std::string binaryStructName_;
};

}  // namespace optimizer

#endif
