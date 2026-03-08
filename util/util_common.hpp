#ifndef OPTIMIZER_UTIL_COMMON_HPP
#define OPTIMIZER_UTIL_COMMON_HPP

/**
 * @file util_common.hpp
 * @brief 軽量ユーティリティ（Handler, RunConfig, CoilList, CoilDataPath, DataConfig, TerminalMessage）
 */

#include <string>
#include <vector>

// --- Handler (global; 設定ファイルで最適化器リストを返す窓口) ---
/**
 * @brief 設定ファイルで指定された最適化器を返す窓口
 *
 * コンストラクタで config を読込み、getOptimizersToRun() で実行する最適化器名のリストを取得する。
 */
class Handler {
public:
    explicit Handler(const std::string& configPath);
    virtual ~Handler();

    /** @brief 実行する最適化器名のリスト（設定の optimizer= で指定、未指定時は PSO,DE,LM） */
    std::vector<std::string> getOptimizersToRun() const;
};

namespace optimizer {

// --- RunConfig ---
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
    /** @brief LM でパラメータの上下限を適用するか（para.cfg の lm_apply_bounds で切り替え） */
    bool lm_apply_bounds = true;
};

// --- CoilList ---
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

// --- CoilDataPath ---
/**
 * @brief コイルバイナリファイルのパス規則と存在チェック
 *
 * ファイル名規則: コイルNO(6桁)_年月日(yyyymmdd).構造体名
 * 例: 000123_20250107.MyStruct
 * コイルNO は 6 桁にゼロパディングする。
 */
namespace CoilDataPath {

/**
 * @brief バイナリファイルのフルパスを組み立てる
 * @param dataPath バイナリのベースディレクトリ（末尾の / はあってもなくてもよい）
 * @param coilNo コイルNO（数字のみ想定。6桁未満はゼロパディング）
 * @param yyyymmdd 年月日 (yyyymmdd)
 * @param binaryStructName 拡張子に使う構造体名（ドットは含めない）
 */
std::string buildPath(const std::string& dataPath,
                      const std::string& coilNo,
                      const std::string& yyyymmdd,
                      const std::string& binaryStructName);

/** @brief ファイルが存在するか */
bool fileExists(const std::string& path);

}  // namespace CoilDataPath

// --- DataConfig ---
/**
 * @brief コイルデータ用設定（para.cfg から読込）
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

// --- TerminalMessage ---
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

#endif  // OPTIMIZER_UTIL_COMMON_HPP
