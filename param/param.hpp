#ifndef OPTIMIZER_PARAM_HPP
#define OPTIMIZER_PARAM_HPP

/**
 * @file param.hpp
 * @brief パラメータ設定とマッパ（ParamSpec, CsvParamLoader, ParameterMapper）
 */

#include <string>
#include <vector>

namespace optimizer {

// --- ParamSpec ---
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

// --- CsvParamLoader ---
/**
 * @brief CSV からパラメータ設定を読み込み・検証する
 *
 * CSV: 1行1パラメータ。列は param_name, enable_opt, init_mode, init_value, db_key, lower, upper, apply_bounds, note。
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

// --- ParameterMapper ---
/**
 * @brief db_key から初期値を返す関数の型
 *
 * USERWORK: 現場DBファイルから db_key に対応する値を取得する実装に差し替える。
 * 本番では実 DB から読み、未指定時は init_value を用いる想定。
 */
using DbValueProvider = double (*)(const std::string& db_key);

/**
 * @brief 全パラメータ集合と最適化ベクトルの対応付け
 *
 * 物理モデル用の全パラメータ（CSV 行順）と、enable_opt された部分だけのベクトルを相互変換する。
 */
class ParameterMapper {
public:
    ParameterMapper() = default;

    /**
     * @brief 検証済み設定から構築（specs の並び = 全パラメータの並び）
     */
    void setSpecs(std::vector<ParamSpec> specs);

    /**
     * @brief 検証で許容するパラメータ名を設定（空なら任意）
     */
    void setAllowedParamNames(std::vector<std::string> names);

    /**
     * @brief 現在の specs を検証
     * @param[out] errorMessage 不正時にメッセージを格納
     * @return 有効なら true
     */
    bool validate(std::string& errorMessage) const;

    /** @brief 最適化ベクトルの次元数 */
    size_t numOptParams() const;
    /** @brief 全パラメータの数 */
    size_t numFullParams() const;

    /**
     * @brief 初期ベクトル（init_mode: manual / db / hybrid に従う）
     * @param dbValueProvider db/hybrid 時に使用。nullptr のときは init_value を使用
     */
    std::vector<double> getInitialVector(DbValueProvider dbValueProvider = nullptr) const;

    /** @brief 最適化ベクトル用の下限（getInitialVector と同じ順） */
    std::vector<double> getLowerBounds() const;
    /** @brief 最適化ベクトル用の上限 */
    std::vector<double> getUpperBounds() const;
    /** @brief 各最適化パラメータで上下限を適用するか（getInitialVector と同じ順） */
    std::vector<bool> getApplyBounds() const;

    /**
     * @brief 最適化ベクトルを全パラメータに展開（specs の並び）
     * USERWORK: どのインデックスがどの物理パラメータかは製品仕様に依存する。
     */
    std::vector<double> expandToFullParameterSet(const std::vector<double>& x_opt) const;

    /** @brief 全パラメータベクトルから最適化ベクトルを抽出 */
    std::vector<double> fullToOptVector(const std::vector<double>& full) const;

    /** @brief 現在の設定リスト */
    const std::vector<ParamSpec>& specs() const { return specs_; }

private:
    std::vector<ParamSpec> specs_;
    std::vector<std::string> allowedParamNames_;
    std::vector<size_t> optIndices_;  /**< 各最適化次元が full の何番目か */
};

}  // namespace optimizer

#endif  // OPTIMIZER_PARAM_HPP
