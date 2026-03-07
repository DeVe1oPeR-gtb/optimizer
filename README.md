# optimizer

既存の最適化ライブラリをベースに、複数製品・共通評価枠・パラメータマッピングを扱うように改良した構成です。

## ディレクトリ構成

```
optimizer/
├── Makefile              # ビルド（CMake は使用しない）
├── run_tests.sh          # テスト実行
├── README.md
├── docs/
│   ├── IMPROVEMENT_PLAN.md   # 改良計画・読解結果
│   └── USERWORK.md           # USERWORK 一覧
├── config/
│   └── params_sample.csv    # サンプル設定 CSV
├── core/                  # 評価インターフェース
│   ├── EvalResult.h, JacobianResult.h
│   ├── IObjective.h, IDifferentiableObjective.h
├── param/                 # パラメータ設定
│   ├── ParamSpec.h, ParamSpec.cpp
│   ├── CsvParamLoader.h/cpp, ParameterMapper.h/cpp
├── product/               # 製品評価
│   ├── ProductMeta.h, ProductLoadedData.h
│   ├── ProductResidualBlock.h, ResidualAssembly.h
│   ├── ProductRunner.h/cpp, BatchEvaluationHandler.h/cpp
├── objective/             # 目的関数
│   ├── Objective.h, Objective.cpp
├── model/                 # 物理モデル・データ読込（モック＋インターフェース）
│   ├── IPhysicalModel.h, MockPhysicalModel.h/cpp
│   ├── IProductDataLoader.h, MockProductDataLoader.h/cpp
├── Optimizer/, PSO/, LM/, DE/, Handler/, util/  # 既存
└── tests/                 # 追加テスト（ParameterMapper, ProductRunner, Batch, Objective, 接続）
```

## 評価フロー

```
Optimizer (PSO/DE/LM)
  → IObjective::evaluate(x) / evaluateWithJacobian(x)
  → Objective
  → ParameterMapper (x_opt → FullParameterSet)
  → BatchEvaluationHandler
  → 各製品: ProductRunner (読込 → 物理モデル → residual block)
  → 全製品の residual を連結 → 返却
```

最適化器から見える評価は常に **residual vector**（`std::vector<double>`）。全製品の residual block を連結したもの。objective スカラはこの vector から計算（例: 二乗和の平方根）。

## 設定 CSV 列定義

| 列名 | 説明 |
|------|------|
| param_name | パラメータ名（空欄禁止） |
| enable_opt | 1 で最適化対象に含める |
| init_mode | manual / db / hybrid |
| init_value | 数値（manual 時や hybrid で DB に無い場合に使用） |
| db_key | DB 参照キー（db / hybrid 時） |
| lower, upper | 上下限（enable_opt=1 のとき必須、lower ≤ upper） |
| note | 備考 |

## 初期値決定ルール

- **manual**: `init_value` を使用。
- **db**: DB から `db_key` に対応する値を取得。  
  **USERWORK**: 実 DB 値ローダを差し込む。
- **hybrid**: DB に値があればそれを使用、なければ `init_value`。

## residual 連結の考え方

- 1 製品ごとに ProductRunner が residual block（`ProductResidualBlock`）を生成。
- BatchEvaluationHandler が全製品をループし、各 block の `residuals` を順に連結して `full_residuals` を構成。
- 各 block の `offset` / `size` で、連結ベクトル内の位置と長さを記録。

## 既存コードからの主な変更点

- 共通評価インターフェース（IObjective, IDifferentiableObjective）の追加。
- ParameterMapper による「最適化ベクトル ⇔ 全パラメータ」の分離と CSV 設定。
- 製品単位の ProductRunner と BatchEvaluationHandler（アルゴリズム非依存）。
- 評価は常に residual vector で返す形に統一。
- 物理モデル・データ読込はインターフェース＋モックとし、USERWORK で差し替え可能に。
- ビルドは Makefile、テストは run_tests.sh。

## ビルド・テスト

```bash
make all
./run_tests.sh
```

CppUTest のパスは `Makefile` の `CPPUTEST_DIR` で変更可能。

## USERWORK 一覧

差し替え・実装が必要な箇所は **docs/USERWORK.md** に一覧しています。コード内は `USERWORK:` で検索してください。

- 実物理モデル本体
- 実製品データローダ
- 実 DB 値ローダ
- FullParameterSet の定義詳細
- 実製品フォーマット依存処理
- 実運用向け objective の詳細
