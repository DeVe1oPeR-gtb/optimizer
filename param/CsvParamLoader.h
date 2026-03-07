#ifndef OPTIMIZER_PARAM_CSV_PARAM_LOADER_H
#define OPTIMIZER_PARAM_CSV_PARAM_LOADER_H

#include "ParamSpec.h"
#include <string>
#include <vector>

namespace optimizer {

/**
 * @brief CSV からパラメータ設定を読み込み・検証する
 *
 * CSV: 1行1パラメータ。列は param_name, enable_opt, init_mode, init_value, db_key, lower, upper, note。
 */
class CsvParamLoader {
public:
    /**
     * @brief ファイルから設定を読み込む
     * @param path CSV ファイルパス
     * @param[out] errorMessage 失敗時にエラー内容を格納
     * @return 読み込んだ ParamSpec の列。失敗時は空
     */
    std::vector<ParamSpec> load(const std::string& path, std::string& errorMessage);

    /**
     * @brief 設定リストを検証する
     * @param specs 検証対象
     * @param allowedParamNames 許容するパラメータ名の一覧。空なら任意
     * @param[out] errorMessage 不正時にエラー内容を格納
     * @return 検証が通れば true
     */
    bool validate(const std::vector<ParamSpec>& specs,
                  const std::vector<std::string>& allowedParamNames,
                  std::string& errorMessage) const;

private:
    static std::string trim(const std::string& s);
    static bool parseRow(const std::string& line, std::vector<std::string>& cells);
};

}  // namespace optimizer

#endif  // OPTIMIZER_PARAM_CSV_PARAM_LOADER_H
