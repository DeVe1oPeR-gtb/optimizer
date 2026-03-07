#ifndef OPTIMIZER_UTIL_COIL_LIST_H
#define OPTIMIZER_UTIL_COIL_LIST_H

#include <string>
#include <vector>

namespace optimizer {

/**
 * @brief 1 件のコイル情報（年月日 + コイルNO）
 */
struct CoilEntry {
    std::string yyyymmdd;   /**< 年月日 (yyyymmdd) */
    std::string coil_no;    /**< コイルNO（6桁にゼロパディングする場合は CoilDataPath 側で実施） */
};

/**
 * @brief xcoil.txt から年月日・コイルNOのリストを読み込む
 *
 * フォーマット: 1行1件。区切りはカンマまたは空白。
 *   yyyymmdd,coil_no  または  yyyymmdd coil_no
 * 行頭の空白は無視。空行・# 以降は無視。
 *
 * @param path xcoil ファイルパス（DataConfig::getXcoilFilePath() 等）
 * @param[out] out 読み込んだ CoilEntry のリスト（追記される。呼び出し前に clear するかどうかは呼び出し側次第）
 * @return 読み込んだ件数。ファイルが開けない場合は -1
 */
int loadCoilListFromFile(const std::string& path, std::vector<CoilEntry>& out);

}  // namespace optimizer

#endif
