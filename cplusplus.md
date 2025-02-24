# C++ 言語仕様書

## 1. 概要

C++ は、C 言語を基盤とした汎用プログラミング言語であり、オブジェクト指向、ジェネリックプログラミング、および関数型プログラミングの機能を備えています。C 言語の拡張として開発され、高いパフォーマンスと柔軟な設計が可能です。C++ は低レベルなメモリ管理を提供しつつ、高度な抽象化も可能にするため、システムプログラミングからアプリケーション開発まで幅広く使用されます。

本仕様書では、C++ の基本構文、データ型、制御構造、関数、クラス、テンプレート、例外処理、標準ライブラリの主要コンポーネント、メモリ管理、マルチスレッド、ファイル入出力などについて説明します。また、最新の C++ 標準に関する情報も含めています。

## 2. 基本構文

### 2.1 コメント

C++ では、コードの可読性を向上させるためにコメントを使用します。

- 一行コメント: `// これはコメントです`
- 複数行コメント: `/* これは複数行のコメントです */`

### 2.2 予約語

C++ には予約語があり、これらは識別子として使用できません。

```cpp
int, return, if, else, while, for, class, public, private, template, virtual, new, delete
```

### 2.3 名前空間

C++ では `namespace` を使用して識別子の衝突を回避します。

```cpp
namespace MyNamespace {
    int myFunction() { return 42; }
}
using namespace MyNamespace;
int main() {
    return myFunction();
}
```

名前空間を適切に利用することで、ライブラリ間の競合を防ぐことができます。

## 3. データ型

C++ には多様なデータ型があり、用途に応じて適切な型を選択することが重要です。

### 3.1 基本データ型

- 整数型: `int, short, long, long long`
- 浮動小数点型: `float, double, long double`
- 文字型: `char`
- 真偽値型: `bool`
- 空型: `void`

### 3.2 修飾子

- `signed` / `unsigned`（符号付き、符号なし）
- `const`（定数）
- `volatile`（変更される可能性がある変数）
- `static`（静的変数）
- `extern`（外部変数）

これらを適切に組み合わせることで、より効率的なコードを書くことができます。

## 4. 変数と定数

### 4.1 変数の宣言と初期化

```cpp
int a = 10;
double pi = 3.14159;
bool isTrue = true;
```

### 4.2 定数

C++ では、変更できない変数を `const` または `constexpr` を使用して定義できます。

```cpp
const int MAX_VALUE = 100;
constexpr double PI = 3.1415926535;
```

`constexpr` はコンパイル時に評価可能な定数として使用できます。

## 5. 制御構造

C++ では、条件分岐やループを用いてプログラムの流れを制御します。

### 5.1 条件分岐

```cpp
if (x > 0) {
    // 正の数
} else if (x == 0) {
    // ゼロ
} else {
    // 負の数
}
```

### 5.2 ループ

```cpp
for (int i = 0; i < 10; i++) {
    std::cout << i << std::endl;
}
while (condition) {
    // 条件が真の間ループ
}
do {
    // 少なくとも一回は実行
} while (condition);
```

## 6. 関数

C++ では、関数を利用してコードの再利用性を向上させます。

### 6.1 関数の定義と呼び出し

```cpp
int add(int a, int b) {
    return a + b;
}
int main() {
    int result = add(5, 10);
}
```

### 6.2 デフォルト引数

デフォルト値を持つ引数を指定することで、関数の呼び出しを簡潔にできます。

```cpp
void printMessage(std::string message = "Hello") {
    std::cout << message << std::endl;
}
```

## 7. クラスとオブジェクト指向

### 7.1 クラスの定義

C++ のクラスは、データと動作をカプセル化します。

```cpp
class Person {
private:
    std::string name;
    int age;
public:
    Person(std::string n, int a) : name(n), age(a) {}
    void introduce() {
        std::cout << "I am " << name << ", " << age << " years old." << std::endl;
    }
};
```

### 7.2 継承

クラスを継承することで、コードの再利用性を高めます。

```cpp
class Employee : public Person {
private:
    std::string company;
public:
    Employee(std::string n, int a, std::string c) : Person(n, a), company(c) {}
    void work() {
        std::cout << name << " works at " << company << std::endl;
    }
};
```

### 7.3 多重継承

C++ では、複数の基底クラスを持つことができます。

```cpp
class A {
public:
    void show() { std::cout << "Class A" << std::endl; }
};
class B {
public:
    void display() { std::cout << "Class B" << std::endl; }
};
class C : public A, public B {
};
```

### 7.4 public, protected, private 継承の比較

| 継承種別 | 基底クラスの `public` メンバ | 基底クラスの `protected` メンバ | 基底クラスの `private` メンバ |
|----------|-------------------------|---------------------------|---------------------------|
| `public` | `public` のまま         | `protected` のまま         | アクセス不可              |
| `protected` | `protected` に変更     | `protected` のまま         | アクセス不可              |
| `private` | `private` に変更       | `private` に変更         | アクセス不可              |

# C++ 言語仕様書

## 1. 概要

C++ は、C 言語を基盤とした汎用プログラミング言語であり、オブジェクト指向、ジェネリックプログラミング、および関数型プログラミングの機能を備えています。C 言語の拡張として開発され、高いパフォーマンスと柔軟な設計が可能です。C++ は低レベルなメモリ管理を提供しつつ、高度な抽象化も可能にするため、システムプログラミングからアプリケーション開発まで幅広く使用されます。

本仕様書では、C++ の基本構文、データ型、制御構造、関数、クラス、テンプレート、例外処理、標準ライブラリの主要コンポーネント、メモリ管理、マルチスレッド、ファイル入出力などについて説明します。また、最新の C++ 標準に関する情報も含めています。

## 2. 基本構文

### 2.1 コメント

C++ では、コードの可読性を向上させるためにコメントを使用します。

- 一行コメント: `// これはコメントです`
- 複数行コメント: `/* これは複数行のコメントです */`

### 2.2 予約語

C++ には予約語があり、これらは識別子として使用できません。

```cpp
int, return, if, else, while, for, class, public, private, template, virtual, new, delete
```

### 2.3 名前空間

C++ では `namespace` を使用して識別子の衝突を回避します。

```cpp
namespace MyNamespace {
    int myFunction() { return 42; }
}
using namespace MyNamespace;
int main() {
    return myFunction();
}
```

名前空間を適切に利用することで、ライブラリ間の競合を防ぐことができます。

## 7. クラスとオブジェクト指向

### 7.4 public, protected, private 継承の比較

| 継承種別 | 基底クラスの `public` メンバ | 基底クラスの `protected` メンバ | 基底クラスの `private` メンバ |
|----------|-------------------------|---------------------------|---------------------------|
| `public` | `public` のまま         | `protected` のまま         | アクセス不可              |
| `protected` | `protected` に変更     | `protected` のまま         | アクセス不可              |
| `private` | `private` に変更       | `private` に変更         | アクセス不可              |

### 7.5 仮想継承

仮想継承を利用することで、ダイヤモンド継承における二重継承問題を解決できます。

```cpp
class Base {
public:
    void show() { std::cout << "Base class" << std::endl; }
};
class Derived1 : virtual public Base {};
class Derived2 : virtual public Base {};
class FinalDerived : public Derived1, public Derived2 {};
```

### 7.6 仮想関数と多態性

C++ では、仮想関数を用いることで動的ポリモーフィズムを実現できます。

```cpp
class Animal {
public:
    virtual void makeSound() { std::cout << "Some sound" << std::endl; }
};
class Dog : public Animal {
public:
    void makeSound() override { std::cout << "Woof!" << std::endl; }
};
```

### 7.7 純粋仮想関数

純粋仮想関数を定義することで、抽象クラスを作成できます。

```cpp
class AbstractBase {
public:
    virtual void pureVirtualFunction() = 0;
};
```

## 8. テンプレート

テンプレートを使用することで、汎用性の高いコードを記述できます。

```cpp
template<typename T>
T add(T a, T b) {
    return a + b;
}
```

## 9. 例外処理

```cpp
try {
    throw std::runtime_error("Error occurred");
} catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
}
```

## 10. 標準ライブラリ

### 10.1 STL コンテナ

- ベクター: `std::vector<int> v;`
- マップ: `std::map<std::string, int> m;`
- セット: `std::set<int> s;`

### 10.2 ストリーム

```cpp
#include <iostream>
std::cout << "Hello, World!" << std::endl;
```

## 11. メモリ管理

### 11.1 動的メモリ確保

C++ では `new` 演算子を使用して動的にメモリを確保し、`delete` を使用して解放します。

```cpp
int* ptr = new int(10);
delete ptr;
```

配列の場合は以下のようにメモリを確保し、`delete[]` を使用して解放します。

```cpp
int* arr = new int[5];
delete[] arr;
```

### 11.2 スマートポインタ

C++11 以降では、`std::unique_ptr` や `std::shared_ptr` などのスマートポインタを使用することで、メモリ管理を安全かつ効率的に行うことができます。

#### `std::unique_ptr`

`std::unique_ptr` は一つのオーナーシップを持ち、スコープを抜けると自動的にメモリを解放します。

```cpp
#include <memory>
std::unique_ptr<int> uptr = std::make_unique<int>(10);
```

#### `std::shared_ptr`

`std::shared_ptr` は複数のオーナーシップを持つことができ、最後の参照がなくなるとメモリが解放されます。

```cpp
#include <memory>
std::shared_ptr<int> sptr1 = std::make_shared<int>(20);
std::shared_ptr<int> sptr2 = sptr1; // 共有所有権を持つ
```

#### `std::weak_ptr`

`std::weak_ptr` は `std::shared_ptr` との循環参照を回避するために使用されます。

```cpp
#include <memory>
std::shared_ptr<int> sptr = std::make_shared<int>(30);
std::weak_ptr<int> wptr = sptr; // 参照はするが所有権は持たない
```

### 11.3 メモリリークの防止

手動で `new` / `delete` を使用するとメモリリークのリスクがありますが、スマートポインタを使用することで回避できます。また、RAII（Resource Acquisition Is Initialization）という設計原則を用いることで、安全なメモリ管理を実現できます。

### 11.4 アライメント制御

C++17 以降では `std::aligned_alloc` を使用してメモリを適切にアライメントさせることができます。

```cpp
#include <cstdlib>
void* ptr = std::aligned_alloc(16, 64); // 16バイト境界で64バイトのメモリを確保
free(ptr);
```

C++ では適切なメモリ管理を行うことで、パフォーマンスを最適化し、バグの発生を防ぐことが可能です。

## 12. マルチスレッド

```cpp
#include <thread>
void task() { std::cout << "Thread is running" << std::endl; }
int main() {
    std::thread t(task);
    t.join();
}
```

## 13. ファイル入出力

```cpp
#include <fstream>
std::ofstream outFile("output.txt");
outFile << "Hello, File!";
outFile.close();
```
