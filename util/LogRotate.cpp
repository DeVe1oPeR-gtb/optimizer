/**
 * @file LogRotate.cpp
 * @brief ログの容量チェックとローテート
 */

#include "util/LogRotate.hpp"
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <string>

#ifdef _WIN32
#include <io.h>
#include <sys/stat.h>
#define stat _stat
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace optimizer {

static size_t getFileSize(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) return 0;
    // ディレクトリやシンボリックリンクは対象外。通常ファイルのサイズだけ返すため。
    if (!S_ISREG(st.st_mode)) return 0;
    return static_cast<size_t>(st.st_size);
}

bool openLogWithRotation(const std::string& path, std::ofstream& out, size_t maxBytes) {
    if (path.empty()) return false;

    // ログが maxBytes を超えていたら .bak に退避してから新規に開く。肥大化を防ぐため。
    if (maxBytes > 0) {
        size_t sz = getFileSize(path);
        if (sz >= maxBytes) {
            std::string bak = path + ".bak";
            (void)std::remove(bak.c_str());
            if (std::rename(path.c_str(), bak.c_str()) != 0)
                (void)std::remove(path.c_str());
        }
    }

    out.open(path, std::ios::out | std::ios::trunc);
    return out.is_open();
}

}  // namespace optimizer
