/**
 * @file ResultOutput.hpp
 * @brief 結果 CSV 書き出しヘルパ。最適化前・最適化後の2タイミングで書き出し。
 * ヘッダと値を追加し、flush でファイル出力。ファイル/合計サイズ上限で超過時は警告してスキップ。
 */

#ifndef OPTIMIZER_UTIL_RESULT_OUTPUT_H
#define OPTIMIZER_UTIL_RESULT_OUTPUT_H

#include "product/ProductRunResult.hpp"
#include "util/ProductLogBuffer.hpp"
#include <cstddef>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace optimizer {

/** 書き出しタイミング（最適化前＝実績、最適化後＝同構造で比較用） */
enum class ResultTiming { BeforeOptimization, AfterOptimization };

/**
 * @brief 結果 CSV 書き出しヘルパ
 *
 * - addColumn(header, value) / addColumn(header, numeric, format): 現在行に列を追加
 * - endRow(): 現在行を確定し、次の行へ
 * - flush(timing): 蓄積した行を CSV に書き出し（タイミングごとのファイル名）
 * - ファイルサイズ・合計サイズ上限を超える場合は警告して書き出しをスキップ（計算は継続）
 * - 詳細データ（製品×点数を行方向）は writeDetailCsv で別扱い
 * - 短い名前: RO, RO::Before, RO::After で利用可
 */
class ResultOutput {
public:
    /** 短い名前で使う用（RO::Before / RO::After） */
    static constexpr ResultTiming Before = ResultTiming::BeforeOptimization;
    static constexpr ResultTiming After = ResultTiming::AfterOptimization;

    ResultOutput() = default;
    ~ResultOutput();

    /** 列を追加（文字列）。書式は使わない。 */
    void addColumn(const std::string& header, const std::string& value);
    /** 列を追加（数値）。format が空なら自動（実数は有効6桁・小数自動、整数は桁指定なし）。 */
    void addColumn(const std::string& header, double value, const std::string& format = "");
    void addColumn(const std::string& header, int value, const std::string& format = "");
    /** 現在行を確定し、次の行を開始 */
    void endRow();
    /** 蓄積した表を CSV で書き出し。サイズ超過時は警告してスキップ。書き出し後は行をクリア（ヘッダは維持）。 */
    void flush(ResultTiming timing);
    /** ヘッダと行をクリア（次の表用） */
    void clear();

    /** タイミングごとのファイル名フォーマット（{timestamp}, {product_id} 等を置換）。前後で同じ format を渡すと同一ファイルに追記する。 */
    void setFilename(ResultTiming timing, const std::string& format);
    /** 汎用の 2 タイミング書き出し用。前と後で同じファイルに書き出す（1回目でヘッダ+行、2回目で行のみ追記）。PLOG は setPLOGFilename のみ使用。 */
    void setFilenameSame(const std::string& format);
    std::string getFilename(ResultTiming timing) const;
    /** ファイル1つあたりの最大バイト数。0でチェックなし。 */
    void setMaxFileBytes(size_t bytes) { maxFileBytes_ = bytes; }
    size_t getMaxFileBytes() const { return maxFileBytes_; }
    /** 合計書き出しの最大バイト数。0でチェックなし。 */
    void setMaxTotalBytes(size_t bytes) { maxTotalBytes_ = bytes; }
    size_t getTotalBytesWritten() const { return totalBytesWritten_; }
    /** 前回 flush でスキップしたか */
    bool lastFlushSkipped() const { return lastFlushSkipped_; }

    // --- PLOG (product log): 1 製品 1 行。add で列追加、endRow で行確定。終了時またはサイズ超過時に flush ---
    void setPLOGFilename(const std::string& format);
    void PLOG_add(const std::string& column, const std::string& value, const std::string& format = "");
    void PLOG_add(const std::string& column, double value, const std::string& format = "");
    void PLOG_add(const std::string& column, int value, const std::string& format = "");
    void PLOG_endRow();
    void flushPLOG();
    void writePLOG(const std::vector<ProductRunResult>& results, const std::string& column_name);  // 従来 API

    // --- LLOG (length log): 製品データを縦に連結・1 ファイル。終了時またはサイズ超過時に flush ---
    void setLLOGFilename(const std::string& format);
    void LLOG_add(const std::string& column, const std::string& value, const std::string& format = "");
    void LLOG_add(const std::string& column, double value, const std::string& format = "");
    void LLOG_add(const std::string& column, int value, const std::string& format = "");
    void LLOG_endRow();
    void flushLLOG();

    // --- DLOG (detail log): 1 製品 1 ファイル。書き込み毎に flush ---
    void setDLOGFilename(const std::string& format);
    void DLOG_beginProduct(const std::string& product_id);
    void DLOG_add(const std::string& column, const std::string& value, const std::string& format = "");
    void DLOG_add(const std::string& column, double value, const std::string& format = "");
    void DLOG_add(const std::string& column, int value, const std::string& format = "");
    void DLOG_endRow();

    void writeLLOG(const std::vector<ProductRunResult>& results,
                   size_t startIndex,
                   size_t maxPoints,
                   const std::string& filenameFormat);
    void writeDLOG(const std::vector<ProductRunResult>& results,
                   size_t startIndex,
                   size_t maxPoints,
                   const std::string& filenameFormat);
    void writeDLOG(const std::vector<ProductRunResult>& results,
                   const std::string& filenameFormat,
                   const std::vector<std::pair<size_t, size_t>>& perProductRanges);
    void writeDetailCsv(const std::vector<ProductRunResult>& results,
                        size_t startIndex,
                        size_t maxPoints,
                        bool oneFile,
                        const std::string& filenameFormat,
                        const std::vector<std::pair<size_t, size_t>>* perProductRanges = nullptr);

private:
    std::vector<std::string> headers_;
    std::vector<std::vector<std::string>> rows_;
    std::vector<std::string> currentRow_;
    std::string filenameBefore_;
    std::string filenameAfter_;
    ProductLogBuffer plog_;
    std::string llogFilename_;
    std::vector<std::string> llogHeaders_;
    std::vector<std::vector<std::string>> llogRows_;
    std::vector<std::string> llogCurrentRow_;
    std::string dlogFilenameFormat_;
    std::string dlogCurrentProductId_;
    std::string dlogCurrentPath_;
    std::vector<std::string> dlogHeaders_;
    std::vector<std::string> dlogCurrentRow_;
    std::ofstream dlogFile_;
    std::string detailFilenameFormat_;
    size_t maxFileBytes_ = 0;
    size_t maxTotalBytes_ = 0;
    size_t totalBytesWritten_ = 0;
    bool lastFlushSkipped_ = false;

    void ensureHeader(const std::string& header);
    std::string formatNumeric(double value, const std::string& format) const;
    std::string formatNumeric(int value, const std::string& format) const;
    std::string replacePlaceholders(const std::string& fmt, const std::string& product_id) const;
    bool wouldExceedLimits(size_t appendBytes, const std::string& path) const;
    void writeCsvToFile(const std::string& path);
    void writeRowsAppend(const std::string& path);
    void appendSummaryRows(const std::vector<ProductRunResult>& results);
    std::string lastPathWithHeader_;
};

/** クラス名の短いエイリアス（RO::Before, RO::After と併用） */
using RO = ResultOutput;
}  // namespace optimizer

#endif
