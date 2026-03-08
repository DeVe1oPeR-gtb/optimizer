#ifndef OPTIMIZER_MOCK_RESULT_WRITER_STUB_H
#define OPTIMIZER_MOCK_RESULT_WRITER_STUB_H

#include "util/IResultWriter.hpp"

namespace optimizer {

/**
 * @brief 結果出力のスタブ（テスト・デモ用）
 *
 * writeApplyOnly / writeAfterOptimization は何も出力しない。
 * USERWORK: 現場では IResultWriter を実装したクラスに差し替え、ファイル・DB 等に書き出す。
 */
class ResultWriterStub : public IResultWriter {
public:
    void writeApplyOnly(const std::vector<double>& fullParams,
                        const std::vector<ProductRunResult>& results) override;

    void writeAfterOptimization(const std::vector<double>& fullParams,
                                const std::vector<ProductRunResult>& results) override;
};

}  // namespace optimizer

#endif
