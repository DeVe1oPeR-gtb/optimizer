#ifndef OPTIMIZER_UTIL_LOG_ROTATE_H
#define OPTIMIZER_UTIL_LOG_ROTATE_H

#include <fstream>
#include <string>

namespace optimizer {

/**
 * @brief ログファイルを容量チェックしてローテートし、書き込み用に開く
 *
 * path のファイルが存在し、サイズが maxBytes 以上なら path.bak にリネームしてから
 * path を新規に開く（上書き）。maxBytes 未満なら path を追記で開く（または上書きで開く）。
 * 本実装では「容量超過時は .bak に退避してから path を上書きで開く」。
 * @param path ログファイルパス
 * @param out 開いたストリームを格納
 * @param maxBytes このバイト数以上でローテート（0 のときはローテートしない）
 * @return 開けたら true
 */
bool openLogWithRotation(const std::string& path, std::ofstream& out, size_t maxBytes);

}  // namespace optimizer

#endif
