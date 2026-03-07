/**
 * @file TerminalMessage.cpp
 * @brief ターミナルへのメッセージ出力（info=stdout, error=stderr, summary=見出し+複数行）。
 */

#include "util/TerminalMessage.h"
#include <iostream>

namespace optimizer {

namespace TerminalMessage {

void info(const std::string& message) {
    std::cout << message;
    if (!message.empty() && message.back() != '\n')
        std::cout << '\n';
    std::cout.flush();
}

void error(const std::string& message) {
    std::cerr << message;
    if (!message.empty() && message.back() != '\n')
        std::cerr << '\n';
    std::cerr.flush();
}

void summary(const std::string& title, const std::vector<std::string>& lines) {
    std::cout << title << '\n';
    for (const auto& line : lines)
        std::cout << "  " << line << '\n';
    std::cout.flush();
}

}  // namespace TerminalMessage

}  // namespace optimizer
