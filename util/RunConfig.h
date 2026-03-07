#ifndef OPTIMIZER_UTIL_RUN_CONFIG_H
#define OPTIMIZER_UTIL_RUN_CONFIG_H

#include <string>
#include <vector>

namespace optimizer {

/**
 * @brief 最適化実行用の設定（ファイルに依存しない注入用）
 *
 * 現場が自前の設定系から詰めて渡せる。未設定の数値はドライバのデフォルトを使用。
 */
struct RunConfig {
    bool trace_enabled = false;
    std::vector<std::string> optimizer_names;  /**< 実行する最適化器（例: {"PSO","DE","LM"}）。空ならデフォルト */
    int n_iter_pso = 120;
    int n_iter_de = 120;
    int n_iter_lm = 80;
};

}  // namespace optimizer

#endif
