#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>
#include <type_traits>

template <std::size_t Rows>
class CSVWriter {

public:
    explicit CSVWriter(const std::string& filename, const std::string& headerFormat = "[]")
        : filename_(filename), headerFormat_(headerFormat) {
        data_.resize(Rows);
    }

    // 1次元配列 (T[Rows]) に対応
    template <typename T>
    void write(const std::string& header, const T (&array)[Rows], const std::string& format = "") {
        static_assert(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, double>,
                      "Only int, float, and double types are allowed");

        // ヘッダの追加
        headers_.push_back(header);

        // 行ごとにデータを追加
        for (size_t r = 0; r < Rows; ++r) {
            data_[r].push_back(formatData(array[r], format));
        }
    }

    // 2次元配列 (T[Rows][Cols])
    template <typename T, std::size_t Cols>
    void write(const std::string& header, const T (&array)[Rows][Cols], const std::string& format = "") {
        static_assert(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, double>,
                      "Only int, float, and double types are allowed");

        // ヘッダの追加
        for (size_t c = 0; c < Cols; ++c) {
            headers_.push_back(generateHeaderName(header, c));
        }

        // 行ごとにデータを追加
        for (size_t r = 0; r < Rows; ++r) {
            for (size_t c = 0; c < Cols; ++c) {
                data_[r].push_back(formatData(array[r][c], format));
            }
        }
    }

    // 3次元配列 (T[Rows][Cols][Depth])
    template <typename T, std::size_t Cols, std::size_t Depth>
    void write(const std::string& header, const T (&array)[Rows][Cols][Depth], const std::string& format = "") {
        static_assert(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, double>,
                      "Only int, float, and double types are allowed");

        // ヘッダの追加
        for (size_t c = 0; c < Cols; ++c) {
            for (size_t d = 0; d < Depth; ++d) {
                headers_.push_back(generateHeaderName(header, c, d));
            }
        }

        // 行ごとにデータを追加
        for (size_t r = 0; r < Rows; ++r) {
            for (size_t c = 0; c < Cols; ++c) {
                for (size_t d = 0; d < Depth; ++d) {
                    data_[r].push_back(formatData(array[r][c][d], format));
                }
            }
        }
    }

    // ヘッダ名を置換
    void replaceHeaders(const std::vector<std::pair<std::string, std::string>>& replacements) {
        bool foundAny = false;
    
        for (const auto& headerPair : replacements) {
            bool foundThis = false;
            for (auto& header : headers_) { // headers_ 内の各ヘッダを1つずつチェック
                if (header == headerPair.first) { // ヘッダが置換対象と一致する場合
                    header = headerPair.second;   // ヘッダを置換
                    foundAny = true;
                    foundThis = true;
                    break; // 1回置換したら次のヘッダへ
                }
            }
            if (!foundThis) {
                std::cerr << "Warning: Header '" << headerPair.first << "' not found in CSV. Skipping replacement.\n";
            }
        }   
        if (!foundAny) {
            std::cerr << "Warning: No matching headers found for replacement.\n";
        }
    }

    // ファイル出力
    void out() {
        std::ofstream file(filename_);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename_ << std::endl;
            return;
        }

        // ヘッダ書き込み
        for (size_t i = 0; i < headers_.size(); ++i) {
            file << headers_[i];
            if (i < headers_.size() - 1) file << ",";
        }
        file << "\n";

        // データ書き込み
        for (const auto& row : data_) {
            for (size_t i = 0; i < row.size(); ++i) {
                file << row[i];
                if (i < row.size() - 1) file << ",";
            }
            file << "\n";
        }

        file.close();
    }

private:
    std::string filename_;
    std::string headerFormat_;
    std::vector<std::string> headers_;
    std::vector<std::vector<std::string>> data_;

    // フォーマットを適用
    template <typename T>
    std::string formatData(T value, const std::string& format) {
        if (format.empty()) {
            return std::to_string(value);
        }

        char buffer[50];
        std::sprintf(buffer, format.c_str(), value);
        return std::string(buffer);
    }

    // ヘッダ名をフォーマットに応じて生成
    std::string generateHeaderName(const std::string& base, size_t i) {
        return (headerFormat_ == "[]") ? base + "[" + std::to_string(i) + "]" : base + "_" + std::to_string(i);
    }

    std::string generateHeaderName(const std::string& base, size_t i, size_t j) {
        return (headerFormat_ == "[]")
                   ? base + "[" + std::to_string(i) + "][" + std::to_string(j) + "]"
                   : base + "_" + std::to_string(i) + "_" + std::to_string(j);
    }
};

// 使用例
// int main() {
//     constexpr size_t Rows = 3;

//     int int2D[Rows][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
//     double double3D[Rows][2][2] = {
//         {{1.123, 2.234}, {3.345, 4.456}},
//         {{5.567, 6.678}, {7.789, 8.890}},
//         {{9.901, 10.012}, {11.123, 12.234}}
//     };

//     CSVWriter<Rows> writer("output.csv");

//     writer.write("Matrix", int2D, "%d");
//     writer.write("Tensor", double3D, "%.3f");

//     // ヘッダ置換
//     std::vector<std::pair<std::string, std::string>> replacements = {
//         {"Matrix[0]", "Column_A"},
//         {"Matrix[1]", "Column_B"},
//         {"Matrix[2]", "Column_C"},
//         {"Tensor[0][0]", "Tensor_A"},
//         {"Tensor[0][1]", "Tensor_B"},
//         {"NonExistingHeader", "Ignored"} // 存在しない場合の警告確認
//     };
    
//     writer.replaceHeaders(replacements);
//     writer.out();

//     std::cout << "CSV file 'output.csv' written successfully!" << std::endl;
//     return 0;
// }
