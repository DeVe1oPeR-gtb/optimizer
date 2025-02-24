# CppUTest を用いたテスト手順書

## 1. CppUTest の導入

### 1.1 CppUTest のインストール

#### 方法1: ソースコードからビルド (推奨)
```sh
# 必要なパッケージのインストール (Linux)
sudo apt install cmake g++ make

# ソースコードの取得
git clone https://github.com/cpputest/cpputest.git
cd cpputest

# ビルド
cmake -B build .
cmake --build build --target install
```

#### 方法2: パッケージマネージャーを使用
**Homebrew (macOS/Linux)**
```sh
brew install cpputest
```

**vcpkg (Windows)**
```sh
vcpkg install cpputest
```

## 2. プロジェクト構成

```
project_root/
├── include/      # 既存のヘッダーファイル
│   ├── my_class.h
│   ├── my_functions.h
│   └── ...
├── src/          # 既存の実装ファイル
│   ├── my_class.cpp
│   ├── my_functions.cpp
│   └── ...
├── test/         # テストコードを格納するディレクトリ (新規作成)
│   ├── CMakeLists.txt
│   ├── main.cpp  # テストエントリーポイント
│   ├── test_my_class.cpp
│   ├── test_my_functions.cpp
│   └── ...
├── CMakeLists.txt
└── README.md
```

## 3. テストコードの作成

### 3.1 テストエントリーポイント (`test/main.cpp`)
```cpp
#include "CppUTest/CommandLineTestRunner.h"

int main(int argc, char** argv) {
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
```

### 3.2 クラスのテスト (`test/test_my_class.cpp`)
```cpp
#include "CppUTest/TestHarness.h"
#include "my_class.h"

TEST_GROUP(MyClassTest) {
    MyClass* myClass;

    void setup() {
        myClass = new MyClass();
    }

    void teardown() {
        delete myClass;
    }
};

TEST(MyClassTest, 初期化テスト) {
    CHECK(myClass != nullptr);
}

TEST(MyClassTest, メソッドのテスト) {
    int result = myClass->someMethod();
    LONGS_EQUAL(42, result);
}
```

### 3.3 関数のテスト (`test/test_my_functions.cpp`)
```cpp
#include "CppUTest/TestHarness.h"
#include "my_functions.h"

TEST_GROUP(MyFunctionsTest) {};

TEST(MyFunctionsTest, 足し算関数のテスト) {
    int result = add(2, 3);
    LONGS_EQUAL(5, result);
}
```

## 4. CMake を使ったビルド

### 4.1 ルートの `CMakeLists.txt`
```cmake
cmake_minimum_required(VERSION 3.10)
project(MyProject)

# C++ 標準の設定
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CppUTest の追加
add_subdirectory(cpputest)

# ソースコードのビルド
target_include_directories(${PROJECT_NAME} PRIVATE include)
add_executable(${PROJECT_NAME} src/my_class.cpp src/my_functions.cpp)
```

### 4.2 `test/CMakeLists.txt`
```cmake
# テスト用の CMakeLists.txt
add_executable(test_runner main.cpp test_my_class.cpp test_my_functions.cpp)
target_include_directories(test_runner PRIVATE ../include)
target_link_libraries(test_runner PRIVATE CppUTest CppUTestExt)
```

### 4.3 ビルドと実行
```sh
mkdir build && cd build
cmake ..
make
./test/test_runner
```

## 5. テスト実行結果

成功時:
```
.
OK (2 tests, 2 ran, 0 checks, 0 ignored, 0 filtered out, 0 ms)
```

失敗時:
```
.
FAIL: test/test_my_class.cpp:14: CHECK failed
```

## 6. CppUTest のアサーション一覧

| アサーション | 説明 |
|-------------|------|
| CHECK(condition) | 条件が `true` であることを確認 |
| CHECK_FALSE(condition) | 条件が `false` であることを確認 |
| LONGS_EQUAL(expected, actual) | `long` 型の値を比較 |
| UNSIGNED_LONGS_EQUAL(expected, actual) | `unsigned long` 型の値を比較 |
| DOUBLES_EQUAL(expected, actual, tolerance) | `double` 型の値を許容範囲付きで比較 |
| STRCMP_EQUAL(expected, actual) | `char*` 文字列の比較 |
| STRNCMP_EQUAL(expected, actual, length) | 指定長さの `char*` 文字列の比較 |
| MEMCMP_EQUAL(expected, actual, size) | メモリブロックの比較 |
| POINTERS_EQUAL(expected, actual) | ポインタの比較 |
| BYTES_EQUAL(expected, actual) | `char` 型の値を比較 |
| FAIL(text) | テストを強制的に失敗させる |
| CHECK_TEXT(condition, text) | 条件が `false` の場合にメッセージを表示 |

これらのアサーションを活用して、より強力なテストを作成してください。

