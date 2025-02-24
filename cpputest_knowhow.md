# CppUTest による Solver クラスのテスト解説 🚀🚀🚀

## 1. 概要

このドキュメントでは、CppUTest を用いて `Solver` クラスの単体テストを実装するコードについて解説します。CppUTest は C++ の軽量なテストフレームワークであり、組み込みシステムのテストにも適しています。📌📌📌

## 2. コードの全体構造

```cpp
#include "Solver.h"

// CppUTest のヘッダーファイル
#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"

TEST_GROUP(Solver)
{
  Solver* solver;

  void setup()
  {
    solver = new Solver();
  }
  void teardown()
  {
    delete solver;
  }
};

TEST(Solver, Create)
{
  enum {
    val1,
    val2,
    val3
  };
  solver->setValue(val1, val2);
  LONGS_EQUAL(val1, solver->get_c1());
  LONGS_EQUAL(val2, solver->get_c2());
}

TEST(Solver, UpdateValues)
{
  enum {
    val1,
    val2,
    val3
  };
  solver->setValue(val1, val2);
  LONGS_EQUAL(val1, solver->get_c1());
  LONGS_EQUAL(val2, solver->get_c2());

  solver->setValue(val2, val3);
  LONGS_EQUAL(val2, solver->get_c1());
  LONGS_EQUAL(val3, solver->get_c2());
}

int main(int argc, char** argv){
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
```

## 3. 各部分の解説 ✍️✍️✍️

### 3.1 インクルード部分

```cpp
#include "Solver.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"
```

- `Solver.h`: テスト対象のクラス `Solver` のヘッダーファイル。
- `CppUTest/TestHarness.h`: CppUTest の基本的なテスト機能を提供。
- `CppUTest/CommandLineTestRunner.h`: コマンドラインからテストを実行するためのヘッダーファイル。

### 3.2 TEST_GROUP(Solver)

```cpp
TEST_GROUP(Solver)
{
  Solver* solver;

  void setup()
  {
    solver = new Solver();
  }
  void teardown()
  {
    delete solver;
  }
};
```

- `TEST_GROUP(Solver)`: `Solver` クラスに関するテストをまとめるグループ。
- `setup()`: 各テストの前に `Solver` のインスタンスを生成。
- `teardown()`: 各テストの後に `Solver` のインスタンスを破棄。

### 3.3 `TEST(Solver, Create)` ✅✅✅

```cpp
TEST(Solver, Create)
{
  enum {
    val1,
    val2,
    val3
  };
  solver->setValue(val1, val2);
  LONGS_EQUAL(val1, solver->get_c1());
  LONGS_EQUAL(val2, solver->get_c2());
}
```

- `TEST(Solver, Create)`: `Solver` クラスのインスタンスが正しく値を設定できるかを確認。
- `enum` を定義し、`setValue()` を呼び出し、設定された値が `get_c1()` と `get_c2()` で取得できるかを検証。
- `LONGS_EQUAL(expected, actual)`: 期待値と実際の値が一致するかをテスト。

### 3.4 `TEST(Solver, UpdateValues)` 🔄🔄🔄

```cpp
TEST(Solver, UpdateValues)
{
  enum {
    val1,
    val2,
    val3
  };
  solver->setValue(val1, val2);
  LONGS_EQUAL(val1, solver->get_c1());
  LONGS_EQUAL(val2, solver->get_c2());

  solver->setValue(val2, val3);
  LONGS_EQUAL(val2, solver->get_c1());
  LONGS_EQUAL(val3, solver->get_c2());
}
```

- `TEST(Solver, UpdateValues)`: `Solver` の値が正しく更新されるかをテスト。
- `setValue()` を 2 回呼び出し、値が変更されることを確認。

### 3.5 `main()` ▶️▶️▶️

```cpp
int main(int argc, char** argv){
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
```

- `CommandLineTestRunner::RunAllTests(argc, argv)`: すべてのテストを実行する。
- コマンドライン引数を受け取り、テスト実行時のオプションを設定可能。

### 3.6 RunAllTests に渡せるオプション 🎛️🎛️🎛️

| オプション      | 説明                         |
| --------------- | ---------------------------- |
| `-v`            | 詳細な出力を表示             |
| `-c`            | コンパクトな出力             |
| `-g TEST_GROUP` | 特定のテストグループのみ実行 |
| `-n TEST_NAME`  | 特定のテストのみ実行         |
| `-o`            | 出力を指定したファイルに保存 |
| `-r`            | リピート実行（回数指定可能） |

## 4. TEST マクロの引数 🎯🎯🎯

```cpp
TEST(TEST_GROUP_NAME, TEST_NAME)
```

| 引数              | 説明                                         |
| ----------------- | -------------------------------------------- |
| `TEST_GROUP_NAME` | テストグループの名前 (`TEST_GROUP()` で定義) |
| `TEST_NAME`       | 個々のテストの名前 (グループ内でユニーク)    |

## 5. まとめ 🎉🎉🎉

このテストコードは `Solver` クラスの `setValue()` の機能を確認するものです。
CppUTest によって、`Solver` クラスの挙動が期待通りであることを簡単に検証できます。
