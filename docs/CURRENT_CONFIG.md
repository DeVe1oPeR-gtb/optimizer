# 現状の設定内容

このドキュメントはプロジェクトの現在の設定を一覧にしたものです。

---

## 1. エディタ / IDE（.vscode/settings.json）

### C/C++ インクルードパス
- `C_Cpp.default.includePath`: ワークスペースルートおよび以下
  - `${workspaceFolder}`
  - `${workspaceFolder}/Optimizer`（PSO/DE/LM は Optimizer/ の下）
  - `${workspaceFolder}/util`
  - `${workspaceFolder}/param`
  - `${workspaceFolder}/product`
  - `${workspaceFolder}/model`
  - `${workspaceFolder}/objective`
  - `${workspaceFolder}/core`

### C++ 標準
- `C_Cpp.default.cppStandard`: `c++17`

### その他
- `files.associations`: 標準ライブラリヘッダを C++ として関連付け
- `files.watcherExclude`: `**/target` をウォッチ対象外

---

## 2. ビルド（Makefile）

| 項目 | 値 |
|------|-----|
| コンパイラ | `g++` |
| C++ 標準 | `-std=c++17` |
| ビルド出力 | `build/` |
| CppUTest ディレクトリ | `CPPUTEST_DIR ?= /home/mcqueen/cpputest` |
| コンパイルオプション | `-g -Wall` + 上記 INC |
| リンク | `-lCppUTest -lCppUTestExt` |

### インクルードパス（コンパイル時）
- `-I.`（ルート）
- `-IOptimizer`（PSO/DE/LM は Optimizer/ 内）, `-Iutil`
- `-Iparam`, `-Iproduct`, `-Imodel`, `-Iobjective`, `-Icore`
- `-I$(CPPUTEST_DIR)/include`

### ライブラリソース
- Optimizer（Optimizer.cpp + Optimizer/PSO/, Optimizer/LM/, Optimizer/DE/）
- param: ParamSpec, CsvParamLoader, ParameterMapper
- model: MockPhysicalModel, MockProductDataLoader
- product: ProductRunner, BatchEvaluationHandler
- objective: Objective

### テスト
- `make all` → `build/AllTests` を生成
- `./run_tests.sh` でテスト実行

---

## 3. パラメータ設定 CSV（config/params_sample.csv）

| 列 | 説明 |
|----|------|
| param_name | パラメータ名 |
| enable_opt | 1=最適化対象 |
| init_mode | manual / db / hybrid |
| init_value | 初期値（数値） |
| db_key | DB 参照キー |
| lower, upper | 下限・上限 |
| note | 備考 |

### サンプル中身（抜粋）
```csv
param_name,enable_opt,init_mode,init_value,db_key,lower,upper,note
p1,1,manual,0.5,,0,1,first param
p2,1,manual,-0.2,,-1,1,second param
p3,0,manual,1.0,,,,
p4,1,hybrid,0.0,db_p4,-0.5,0.5,hybrid init
```

---

## 4. 出力日時

上記は `docs/CURRENT_CONFIG.md` を生成した時点の設定です。  
実際の値は各ファイルを直接参照してください。
