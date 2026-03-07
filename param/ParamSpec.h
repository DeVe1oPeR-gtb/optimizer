#ifndef OPTIMIZER_PARAM_PARAM_SPEC_H
#define OPTIMIZER_PARAM_PARAM_SPEC_H

#include <string>

namespace optimizer {

/**
 * @brief パラメータの初期値の取り方
 */
enum class InitMode {
    Manual,   /**< init_value のみ使用 */
    Db,       /**< DB 値（db_key）を使用 */
    Hybrid    /**< DB にあれば DB 値、なければ init_value */
};

/**
 * @brief パラメータ設定の1行分（1パラメータ）
 */
struct ParamSpec {
    std::string param_name;      /**< パラメータ名 */
    int enable_opt = 0;          /**< 1 のとき最適化ベクトルに含める */
    InitMode init_mode = InitMode::Manual;
    double init_value = 0.0;     /**< 初期値（manual 時など） */
    std::string db_key;          /**< DB 参照キー（db / hybrid 時） */
    double lower = 0.0;          /**< 下限 */
    double upper = 0.0;          /**< 上限 */
    std::string note;            /**< 備考 */
    bool apply_bounds = true;    /**< 最適化時に上下限を適用するか */

    /** @brief 有効な bounds を持つか（enable 時は別途バリデーション） */
    bool hasBounds() const {
        return enable_opt != 0;
    }
};

/**
 * @brief init_mode 文字列を列挙に変換
 * @param s "manual" / "db" / "hybrid"（大文字可）
 * @return 対応する InitMode。未対応なら Manual
 */
InitMode parseInitMode(const std::string& s);

}  // namespace optimizer

#endif  // OPTIMIZER_PARAM_PARAM_SPEC_H
