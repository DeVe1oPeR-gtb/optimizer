/**
 * @file IterationLog.cpp
 * @brief 最適化の反復ログをターミナルに出力するヘルパ（iter, score, position）。
 */

#include "util/IterationLog.h"
#include <iostream>

namespace optimizer {

void logIterationTo(int iteration, double score,
                    const std::vector<double>& position,
                    std::ostream& out,
                    const char* label) {
    /* フォーマットは必要に応じてここで変更してください */
    if (label && *label)
        out << "[" << label << "] ";
    out << "iter=" << iteration << " score=" << score;
    if (!position.empty()) {
        out << " pos=(";
        for (size_t i = 0; i < position.size(); ++i) {
            if (i > 0) out << ",";
            out << position[i];
        }
        out << ")";
    }
    out << "\n";
}

void logIteration(int iteration, double score,
                  const std::vector<double>& position,
                  const char* label) {
    logIterationTo(iteration, score, position, std::cout, label);
}

}  // namespace optimizer
