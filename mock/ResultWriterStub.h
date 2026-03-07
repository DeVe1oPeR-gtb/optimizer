#ifndef OPTIMIZER_MOCK_RESULT_WRITER_STUB_H
#define OPTIMIZER_MOCK_RESULT_WRITER_STUB_H

#include "util/IResultWriter.h"

namespace optimizer {

/**
 * @brief 結果出力のスタブ（オンサイトで IResultWriter を実装するまでの仮実装）
 *
 * writeApplyOnly / writeAfterOptimization は何も出力しない。
 * 現場では IResultWriter を実装し、ファイル・DB 等に書き出す。
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
