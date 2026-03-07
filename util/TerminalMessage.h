#ifndef OPTIMIZER_UTIL_TERMINAL_MESSAGE_H
#define OPTIMIZER_UTIL_TERMINAL_MESSAGE_H

#include <string>
#include <vector>

namespace optimizer {

/**
 * @brief ターミナルへのメッセージ出力ユーティリティ
 *
 * 通常メッセージは stdout、エラーは stderr に出力する。
 */
namespace TerminalMessage {

/** @brief 通常の情報メッセージ（stdout） */
void info(const std::string& message);

/** @brief エラーメッセージ（stderr） */
void error(const std::string& message);

/**
 * @brief 概要をブロックで表示（タイトル + 複数行）
 * @param title 見出し（例: "コイルデータ読込概要"）
 * @param lines 表示する行のリスト（先頭に "  " を付けて表示）
 */
void summary(const std::string& title, const std::vector<std::string>& lines);

}  // namespace TerminalMessage

}  // namespace optimizer

#endif
