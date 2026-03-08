# optimizer 改良計画（読解結果・方針・構成案）

## 1. 既存コードの読解結果

### 1.1 Optimizer（基底）

- **責務**: 最適化アルゴリズムの共通基底。評価データの統計計算のみ実装。
- **構造**:
  - `Stats<T>`: mean, std_dev, rmse
  - `computeStats(eval_data)`: `std::vector<std::pair<T,T>>`（実測・予測のペア）から統計を計算
  - `setParam(const std::vector<double>&)` は空実装
- **所見**: 目的関数のインターフェース（`evaluate(x)` 等）は未整備。アルゴリズム共通の「実行インターフェース」はない。

### 1.2 PSO

- **責務**: 粒子群最適化。粒子ごとの position / velocity / best 管理と RMSE ベースのスコア更新。
- **構造**:
  - `Particle`: position, velocity, best_position, my_score, best_score, eval_data（実測・予測ペア）, stats
  - `Gbest`: 全局最良の particle 番号・位置・スコア・統計
  - コンストラクタ: 粒子数, 次元数, w/c1/c2, 上下限
- **ループの持ち方**: **呼び出し側**が「粒子ループ」を回し、各粒子ごとに `setEvalData(粒子番号, eval_data)` → `calcPersonalScore` → `updatePersonalBest(スコア)` を呼ぶ。最後に `updateGlobalBest()` → `updateParticles()`。
- **評価の形**: 1粒子＝1組の eval_data（実測・予測ペア列）。スコアは `Stats::rmse`。

### 1.3 LM（Levenberg–Marquardt）

- **責務**: ヤコビアンと評価データに基づくガウス・ニュートン的なステップ計算。
- **構造**:
  - コンストラクタ: num_dim, num_data, 摂動スケール z
  - `J_` [num_data][num_dim], `eval_data_`（実測・予測ペア）
  - `setEvalData(eval_data)`: 基準点での残差情報
  - `setJacobian(idx_param, dfdz)`: パラメータ d 方向の微分（データ点数分のベクトル）
  - `getNextDelta()`: 近似ヘッセ＋λI を解いて delta を返す
- **ループの持ち方**: **呼び出し側**が「基準点での評価」＋「パラメータ方向ごとの摂動評価」を実施。基準点で `setEvalData`、各 d で摂動→評価→`setJacobian(d, dfdz)` の順。その後 `getNextDelta()` で delta を取り、呼び出し側で z += delta。
- **評価の形**: 1データセット＝1組の eval_data。LM は「データ点数×パラメータ数」のヤコビアンと残差ベクトルを内部で保持。

### 1.4 DE（差分進化）

- **責務**: 個体群の変異・交叉・選択。RMSE をスコアとして選択。
- **構造**:
  - `Individual`: position, score, stats
  - population_ と mutants_。評価対象は mutants_。
  - コンストラクタ: 個体数, 次元数, F, CR, 上下限
- **ループの持ち方**: **呼び出し側**が「個体（mutant）ループ」を回し、各 n で `setEvalData(n, eval_data)` → `calcScore(n)`。その後 `selection()` → `mutation()`。
- **評価の形**: 1個体＝1組の eval_data。スコアは `stats.rmse`。

### 1.5 Handler

- **責務**: ヘッダのみ存在。アルゴリズム名・ファイル名と pre/post 処理の宣言があるが未実装。`string` が無修飾でコンパイルエラーになる可能性。
- **所見**: 今回の「BatchEvaluationHandler」とは別物。既存 Handler は使わず、新規に BatchEvaluationHandler を「アルゴリズム非依存」で定義する。

### 1.6 util（csvwriter.hpp）

- **責務**: CSV **出力**用。行数固定の 1/2/3 次元配列をヘッダ付きで書き出す。
- **所見**: 設定用 CSV **読込**（param_name, enable_opt, init_mode 等）は未実装。ParameterMapper 用の読込は新規実装。

---

## 2. 現状の問題点（要約）

| 項目 | 現状 | 目標 |
|------|------|------|
| 目的関数の型 | なし。各アルゴリズムが eval_data（実測・予測ペア）を直接セット | IObjective::evaluate(x), IDifferentiableObjective::evaluateWithJacobian(x) |
| 返す評価値 | アルゴリズムごとに Stats や内部状態 | 常に residual vector（全製品連結）＋必要なら objective スカラ |
| ループ構造 | PSO/DE は「候補×製品」、LM は「基準点＋パラメータ方向×製品」で異なる | この違いは Optimizer/Objective 側で吸収。Handler はアルゴリズム非依存 |
| パラメータ | 全次元をそのまま扱い、CSV や enable_opt なし | ParameterMapper で部分ベクトル化・bounds・初期値（manual/db/hybrid） |
| 製品・モデル | 単一データセット想定、モデル抽象なし | 複数製品、1製品1実行の ProductRunner、IPhysicalModel / データローダはモック＋USERWORK |

---

## 3. 差分改良の基本方針

1. **既存 PSO/LM/DE のコアは維持**  
   - 内部の数式・ループ・行列計算は極力そのまま。  
   - 変更するのは「評価の入口」：これまで `setEvalData` で渡していた中身を、Objective が「x → residual vector」で返す形に寄せる。

2. **評価は常に residual vector**  
   - 最適化器から見える評価は `std::vector<double>`（全製品の residual を連結）。  
   - objective スカラは「residual の二乗和」等、この vector から算出。

3. **ループ構造の違いは Handler に押し込めない**  
   - BatchEvaluationHandler: 製品リストを受け、各製品で ProductRunner を 1 回ずつ呼び、residual block を連結して返すだけ。  
   - 「候補解×製品」や「基準点＋パラメータ方向×製品」のループは、PSO/DE 用 Objective や LM 用 Objective（evaluateWithJacobian）側で回す。

4. **数値型は double 中心**  
   - 新規インターフェース・ParameterMapper・製品評価系は double。  
   - 既存 PSO/LM/DE のテンプレートは残してもよいが、明示的インスタンス化はやめる（double のみインスタンス化するか、ヘッダのみでよい）。

5. **ビルドは Makefile または build.sh**  
   - CMake は使わない。run_tests.sh でテスト実行を整備。

6. **物理モデル・データ読込はモック＋USERWORK**  
   - インターフェースだけ定義し、実装はモック。差し替え箇所は USERWORK で明示。

---

## 4. 予定ディレクトリ構成

```
optimizer/
├── Makefile              # または build.sh でビルド
├── run_tests.sh          # テスト実行
├── README.md             # 構成・フロー・CSV・USERWORK 一覧
├── docs/
│   ├── IMPROVEMENT_PLAN.md   # 本ドキュメント
│   └── USERWORK.md           # USERWORK 一覧（README と重複可）
├── config/
│   └── params_sample.csv     # サンプル設定 CSV
├── core/                  # 評価インターフェース・結果型
│   ├── EvalResult.h
│   ├── JacobianResult.h
│   ├── IObjective.h
│   └── IDifferentiableObjective.h
├── param/
│   ├── ParameterMapper.h
│   ├── ParameterMapper.cpp
│   ├── ParamSpec.h         # 1 行分の CSV 行を表す構造
│   └── CsvParamLoader.h/cpp # CSV 読込・バリデーション（必要なら分離）
├── product/               # 製品評価系
│   ├── ProductMeta.h
│   ├── ProductLoadedData.h
│   ├── ProductResidualBlock.h
│   ├── ResidualAssembly.h
│   ├── ProductRunner.h
│   ├── ProductRunner.cpp
│   ├── BatchEvaluationHandler.h
│   └── BatchEvaluationHandler.cpp
├── objective/
│   ├── Objective.h         # 共通 evaluate(x) / evaluateWithJacobian(x) の実装
│   └── Objective.cpp
├── model/                 # 物理モデル・データローダ（モック＋インターフェース）
│   ├── IPhysicalModel.h
│   ├── MockPhysicalModel.h/cpp
│   ├── IProductDataLoader.h
│   └── MockProductDataLoader.h/cpp
├── Optimizer/             # 既存
│   ├── Optimizer.h
│   └── Optimizer.cpp
├── PSO/
│   ├── PSO.h
│   └── PSO.cpp
├── LM/
│   ├── LM.h
│   └── LM.cpp
├── DE/
│   ├── DE.h
│   └── DE.cpp
├── Handler/               # 既存（未使用のままでも可。Batch は product/ に配置）
│   └── Handler.h
├── util/
│   └── csvwriter.hpp
├── tests/
│   ├── test_ParameterMapper.cpp
│   ├── test_ProductRunner.cpp
│   ├── test_BatchEvaluationHandler.cpp
│   ├── test_Objective.cpp
│   └── test_OptimizerConnection.cpp
├── AllTests.cpp
└── AllTests.h
```

（Handler は既存のまま残し、新規の「バッチ評価」は product/BatchEvaluationHandler に集約する形を想定。）

---

## 5. 主要クラス案

### 5.1 共通評価インターフェース（core/）

- **EvalResult**  
  - `double objective`  
  - `std::vector<double> residuals`

- **JacobianResult**  
  - `std::vector<double> residuals`  
  - `std::vector<std::vector<double>> jacobian`（または [residual_index][param_index] の 2 次元）

- **IObjective**  
  - `EvalResult evaluate(const std::vector<double>& x);`

- **IDifferentiableObjective**  
  - 継承: IObjective  
  - `JacobianResult evaluateWithJacobian(const std::vector<double>& x);`

### 5.2 ParameterMapper（param/）

- CSV 読込結果を保持する構造（ParamSpec: param_name, enable_opt, init_mode, init_value, db_key, lower, upper, note）。
- **役割**:
  - `getOptimizationVector()`: enable_opt に従った x_opt の次元・順序
  - `getInitialVector()`: init_mode（manual/db/hybrid）に従った初期値
  - `getLowerBounds()` / `getUpperBounds()`: enable_opt なパラメータの bounds
  - `expandToFullParameterSet(const std::vector<double>& x_opt)`: 全パラメータのベクトル（FullParameterSet）を返す
- 未知パラメータ名・重複・bounds 欠損・lower>upper・初期値が bounds 外などのバリデーションを実装。

### 5.3 製品評価系（product/）

- **ProductMeta**: product_id, file_path 等の軽量メタ情報。
- **ProductLoadedData**: 1 製品分ロード後のデータ（モックではダミー構造でよい）。
- **ProductResidualBlock**: product_id, offset, size, residuals, ok/error。
- **ResidualAssembly**: full_residuals（連結）, blocks（各 ProductResidualBlock）。
- **ProductRunner**:  
  - 1 製品分のデータ読込（ローダ呼び出し）、1 製品分の物理モデル実行、実測との差分から residual block 作成。  
  - インターフェース: 例 `ProductResidualBlock run(const ProductMeta& meta, const ProductLoadedData& loaded, const FullParameterSet& params, IPhysicalModel& model)`。  
  - USERWORK: 実データ読込・実モデル呼び出し。
- **BatchEvaluationHandler**:  
  - 製品リストと ParameterMapper を前提に、FullParameterSet を受け、全製品について ProductRunner を呼び、ResidualAssembly を返す。ループは「製品」のみで、アルゴリズム別の候補ループは持たない。

### 5.4 Objective（objective/）

- **Objective**（IObjective 実装）:
  - 内部に ParameterMapper と BatchEvaluationHandler（および ProductRunner に渡すモデル・ローダの差し替えポイント）を保持。
  - `evaluate(x)`:  
    1) x を ParameterMapper で FullParameterSet に展開  
    2) BatchEvaluationHandler で全製品評価  
    3) full_residuals から objective（例: 二乗和）を計算し、EvalResult で返す。
- **DifferentiableObjective** または同じクラスで IDifferentiableObjective も実装:
  - `evaluateWithJacobian(x)`: 数値微分。基準点 x で residual、各 j で x+dx_j で residual を取得し、Jacobian を組み立てて JacobianResult で返す。

### 5.5 既存 PSO / DE / LM の接続

- **PSO**:  
  - 外からは「初期 x_opt、bounds、options」を渡し、内部で粒子ループ。  
  - 各粒子の position に対して `objective.evaluate(position)` を呼び、返ってきた EvalResult::objective（または residuals から計算したスカラ）で updatePersonalBest 等。  
  - 既存の setEvalData(粒子番号, eval_data) の代わりに、その粒子の position で evaluate を 1 回呼ぶ形に差し替え。
- **DE**:  
  - 同様に、各 mutant の position で `objective.evaluate(position)` を 1 回呼び、返したスコアで setEvalData 相当・calcScore 相当を行う。
- **LM**:  
  - IDifferentiableObjective を使う。  
  - 反復ごとに `evaluateWithJacobian(x)` を 1 回呼び、返ってきた residuals と jacobian を既存 LM の setEvalData / setJacobian に渡す（または LM 側を「residual vector と Jacobian を一度にセット」する API に拡張）。

理想形は `optimizer.run(objective, initial_x, options)` だが、既存構造を壊さない範囲で、まずは「Objective を渡して evaluate だけ統一」する段階から進める。

---

## 6. USERWORK 一覧案

| 番号 | 対象 | 内容 | コード上の目印 | 入出力・ファイル |
|------|------|------|----------------|------------------|
| 1 | 実物理モデル本体 | 1 製品分の入力から 1 製品分の予測ベクトルを返す計算に差し替え | USERWORK: 実物理モデルの計算関数に置き換える | 入力: 1 製品用パラメータ・データ 出力: 予測ベクトル |
| 2 | 実製品データローダ | 製品バイナリ/ファイルの読込と ProductLoadedData の構築 | USERWORK: 実際の製品バイナリファイル読込処理をここに実装する | 入力: ProductMeta（file_path 等） 出力: ProductLoadedData |
| 3 | 実 DB 値ローダ | init_mode=db / hybrid のとき、db_key に対応する値を返す | USERWORK: 現場 DB ファイルから db_key に対応する値を取得する | 入力: db_key 出力: double 値 |
| 4 | FullParameterSet の定義詳細 | 物理モデルが受け取る「全パラメータ」の型・並び・意味 | USERWORK: 実際の FullParameterSet の定義（パラメータ並び・単位）を製品仕様に合わせて定義する | モデルと ParameterMapper の橋渡し構造 |
| 5 | 実製品フォーマット依存処理 | ファイル形式や測点並びに依存する読込・残差並び | USERWORK: 実製品フォーマットに合わせた読込・residual 並びを実装する | 上記 2 と連動 |
| 6 | 実運用向け objective の詳細 | 重み付け・評価式・異常値処理など | USERWORK: 実運用向けの目的関数式・重み付けルールをここで実装する | evaluate 内の objective 計算式 |

README および docs/USERWORK.md に上記を「USERWORK 一覧」として記載し、各ファイル内では `// USERWORK: …` で同じ番号または内容を参照する。

---

## 7. 実装ステップ案

| フェーズ | 内容 | 成果物 |
|----------|------|--------|
| 1 | 共通型・インターフェース | core/EvalResult, JacobianResult, IObjective, IDifferentiableObjective |
| 2 | 設定 CSV 仕様と ParameterMapper | ParamSpec, CSV 読込・バリデーション, ParameterMapper（getInitialVector, getBounds, expandToFull） |
| 3 | 製品評価系の型とモック | ProductMeta, ProductLoadedData, ProductResidualBlock, ResidualAssembly, IPhysicalModel, MockPhysicalModel, IProductDataLoader, MockProductDataLoader |
| 4 | ProductRunner と BatchEvaluationHandler | ProductRunner（モック利用）, BatchEvaluationHandler（全製品ループ・連結） |
| 5 | Objective | Objective::evaluate(x), evaluateWithJacobian(x)（数値微分）、ParameterMapper と Batch の組み合わせ |
| 6 | 既存 PSO/DE/LM の接続 | 各アルゴリズムから IObjective/IDifferentiableObjective を呼ぶラッパーまたは run の追加、既存テストが通るようにする |
| 7 | ビルド・テスト整備 | Makefile または build.sh, run_tests.sh, サンプル CSV, README と USERWORK ドキュメント |
| 8 | テスト追加 | ParameterMapper, ProductRunner, BatchEvaluationHandler, Objective, Optimizer 接続の各テスト |

この順で段階的に実装し、既存テストを維持しつつ新テストを追加する。
