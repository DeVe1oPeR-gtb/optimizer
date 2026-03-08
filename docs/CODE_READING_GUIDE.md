# 成果物コード読解ガイド（プログラマ向け）

最適化ライブラリの**読む順番**と、各モジュールの**役割・チェック観点**をまとめた説明書です。コードレビューやオンサイト組み込み前の確認に使ってください。

---

## 1. 推奨読む順番（3 段階）

### Phase 1: データ型とインターフェース（依存の根から）

最適化の「入出力」と「差し替えポイント」を押さえます。

| 順 | パス | 役割 |
|---|------|------|
| 1 | **core/core.hpp** | EvalResult, JacobianResult, IObjective, IDifferentiableObjective（評価結果と目的関数 API） |
| 2 | **product/ProductMeta.hpp** | 製品メタ（ID・パス等） |
| 3 | **product/ProductLoadedData.hpp** | 1 製品分ロード済みデータ（measured, positions） |
| 4 | **product/ProductRunResult.hpp** | 1 製品分の計算結果（実測・予測・残差） |
| 5 | **product/ProductResidualBlock.hpp** | 1 製品分の残差ブロック（offset/size + residuals） |
| 6 | **product/ResidualAssembly.hpp** | 全製品の連結残差 + ブロック一覧 |
| 7 | **model/IPhysicalModel.hpp** | 物理モデル API：`run(fullParams, productLoadedData)` |
| 8 | **model/IProductDataLoader.hpp** | 製品データ読込 API：`load(meta)` |

**チェック観点（Phase 1）**

- 残差の定義が「実測 − 予測」で一貫しているか
- 現場差し替え箇所（USERWORK）がインターフェースで明確か
- ヘッダのみで完結し、不要な依存が入っていないか

---

### Phase 2: パラメータ・製品評価・目的関数（評価パイプライン）

「最適化ベクトル → 全パラメータ → 残差」の流れを追います。

| 順 | パス | 役割 |
|---|------|------|
| 9 | **param/param.hpp** | ParamSpec, CsvParamLoader, ParameterMapper（1 パラメータ設定・CSV 読込・最適化ベクトル ⇔ 全パラメータ） |
| 10 | **param/ParamSpec.cpp**, **CsvParamLoader.cpp**, **ParameterMapper.cpp** | 上記の実装（parseInitMode 等） |
| 11 | **product/ProductRunner.hpp**, **.cpp** | 1 製品の評価：load → model.run → 残差ブロック作成（measured - predicted） |
| 12 | **product/BatchEvaluationHandler.hpp**, **.cpp** | 製品リストをループし、全ブロックを連結して ResidualAssembly を返す |
| 13 | **objective/Objective.hpp**, **.cpp** | IObjective 実装。evaluate: expandToFull → batch.evaluate → 目的値は残差のノルム。evaluateWithJacobian: 数値微分 |

**チェック観点（Phase 2）**

- ParameterMapper の optIndices_ と full の並びが specs と一致しているか
- 初期値・上下限・apply_bounds が CSV/ParamSpec から一貫して渡っているか
- Objective の数値微分の ε と、LM のスケールの相性（桁落ち・ノイズ）
- 複数製品時の full_residuals の並びと offset/size の整合

---

### Phase 3: 最適化器と実行窓口（アルゴリズム・ドライバ）

アルゴリズムの呼び方と、現場が触る API を押さえます。

| 順 | パス | 役割 |
|---|------|------|
| 19 | **Optimizer/Optimizer.h**, **.cpp** | 基底クラス。評価データ (実測, 予測) ペアから Stats（平均・標準偏差・RMSE）を計算 |
| 20 | **Optimizer/PSO/PSO.h**, **.cpp** | 粒子群。getParticles / setEvalData / calcPersonalScore / updatePersonalBest / updateGlobalBest / getGlobalBest。bounds はコンストラクタで受け取り粒子をクリップ |
| 21 | **Optimizer/DE/DE.h**, **.cpp** | 差分進化。getPopulation / setEvalData / calcScore / mutation / selection。bounds は ul_bound_ / ll_bound_ でクリップ |
| 22 | **Optimizer/LM/LM.h**, **.cpp** | Levenberg–Marquardt。setEvalData / setJacobian / getNextDelta。setLambda / getLambda で adaptive λ。gjm で (J^T J + λI) δ = -J^T r を解く |
| 23 | **util/RunConfig.h** | 実行設定（trace, optimizer 名リスト, 反復数, lm_apply_bounds） |
| 24 | **util/TraceConfig.h**, **.cpp** | 開発者設定の読込（trace=on/off, optimizer=..., lm_apply_bounds=on/off）。load(configPath) と loadFromStruct(RunConfig) |
| 25 | **util/IResultWriter.h** | 結果出力の窓口：writeApplyOnly / writeAfterOptimization |
| 26 | **util/ProductLogBuffer.h**, **.cpp** | PLOG 専用バッファ。1 製品 1 行・列を横に追加。ResultOutput が flush 時にファイル書き出しに利用 |
| 27 | **util/ResultOutput.h**, **.cpp** | 結果 CSV ヘルパ。汎用の addColumn/endRow/flush(timing)、PLOG（writePLOG → 終了時 flushPLOG）、LLOG/DLOG（writeLLOG/writeDLOG）。ファイル・合計サイズ上限で超過時は警告してスキップ |
| 28 | **util/OptimizerDriver.h**, **.cpp** | **メイン窓口**。run(configPath または RunConfig, mapper, model, loader, products, optimizerName, ...) で 1 回実行。内部で runPSO / runDE / runLM を分岐。runLM では adaptive lambda・ステップ採用/却下・bounds クリップ。runApplyOnly は最適化なしで適用値のみ計算して IResultWriter に渡す |
| 29 | **util/Handler.h**, **.cpp** | 設定ファイルを読んで getOptimizersToRun() を返す（デモ用） |
| 30 | **util/IterationLog.h**, **.cpp** | 反復ログ出力のヘルパ |

**チェック観点（Phase 3）**

- PSO/DE は「候補解 × 製品」で evaluate を何回も呼ぶ。LM は「現在点 + ヤコビ」で 1 反復あたり 1〜数回。ドライバ側のループの違いを理解しているか
- LM の eval_data の解釈：first=実測, second=予測 で J^T (measured - predicted) が正しい向きか
- runLM の bounds クリップが「適用有無」フラグ（TraceConfig::isLmApplyBoundsEnabled, mapper.getApplyBounds）と整合しているか
- Gauss–Jordan（gjm）のピボットなし・0 割の扱い（LM の数値安定性）

---

### Phase 4: デモ・テスト・モック（動作確認と差し替え例）

| 順 | パス | 役割 |
|---|------|------|
| 31 | **mock/Demo.h**, **Demo.cpp** | デモ用 3 モデル（quadratic, linear, rational_exp）と 3 ローダ（measured を式で生成）。makeSpecs は demo_main 側にあり、初期値 (1, -0.5, 0.1) 等 |
| 32 | **mock/demo_main.cpp** | エントリ。Handler で設定読込 → 3 モデル × 最適化器で OptimizerDriver::run を呼び、result/summary.csv を出力 |
| 33 | **mock/Mock.h**, **Mock.cpp** | テスト用 IPhysicalModel / IProductDataLoader |
| 34 | **mock/ResultWriterStub.h**, **.cpp** | IResultWriter のスタブ（必要なら） |
| 35 | **tests/test_*.cpp** | ParameterMapper, ProductRunner, BatchEvaluationHandler, Objective, OptimizerConnection, ResultOutput, TraceConfig の単体テスト |
| 36 | **Optimizer/*/test_*.cpp** | PSO, DE, LM のユニットテスト |

**チェック観点（Phase 4）**

- デモのモデル・ローダが「残差 = measured - predicted」と ParamSpec の並びと一致しているか
- テストがインターフェースの契約（空リスト・境界値）を押さえているか

---

## 2. モジュール別概要（一覧）

### core/

| ファイル | 概要 |
|----------|------|
| **EvalResult.h** | 評価 1 回分：`objective`（スカラ）, `residuals`（全製品連結） |
| **JacobianResult.h** | 残差 + `jacobian[residual_i][param_j]` |
| **IObjective.h** | `evaluate(x)` → EvalResult |
| **IDifferentiableObjective.h** | IObjective を継承し、`evaluateWithJacobian(x)` → JacobianResult |

### model/

| ファイル | 概要 |
|----------|------|
| **IPhysicalModel.h** | `run(fullParams, productLoadedData)` → 予測ベクトル（実測と同長） |
| **IProductDataLoader.h** | `load(ProductMeta)` → ProductLoadedData（measured, positions） |

### product/

| ファイル | 概要 |
|----------|------|
| **ProductMeta.h** | product_id, file_path |
| **ProductLoadedData.h** | measured, positions（同長） |
| **ProductResidualBlock.h** | product_id, offset, size, residuals, ok, error_message |
| **ResidualAssembly.h** | full_residuals（連結）, blocks（ProductResidualBlock の列） |
| **ProductRunner.h/.cpp** | load(meta) → model.run(fullParams, data) → 残差 = measured - predicted → ProductResidualBlock |
| **BatchEvaluationHandler.h/.cpp** | setProducts(metas); evaluate(fullParams) → 全製品のブロックを連結した ResidualAssembly |
| **ProductRunResult.h** | 1 製品の measured, predicted, residuals（出力用） |

### param/

| ファイル | 概要 |
|----------|------|
| **ParamSpec.h** | param_name, enable_opt, init_mode, init_value, db_key, lower, upper, note, apply_bounds |
| **ParamSpec.cpp** | parseInitMode("manual"|"db"|"hybrid") |
| **CsvParamLoader.h/.cpp** | load(path) → ParamSpec 列。validate(specs, allowedParamNames)。列：param_name, enable_opt, init_mode, init_value, db_key, lower, upper, apply_bounds, note |
| **ParameterMapper.h/.cpp** | setSpecs; getInitialVector, getLowerBounds, getUpperBounds, getApplyBounds; expandToFullParameterSet, fullToOptVector |

### objective/

| ファイル | 概要 |
|----------|------|
| **Objective.h/.cpp** | ParameterMapper + BatchEvaluationHandler で IObjective 実装。evaluate: full = expandToFull(x), asm = batch.evaluate(full), objective = ||residuals||。evaluateWithJacobian: 数値微分（epsilon_ * (1+|x_j|)） |

### Optimizer/

| ファイル | 概要 |
|----------|------|
| **Optimizer.h/.cpp** | 基底。computeStats(eval_data) で平均・標準偏差・RMSE。eval_data は (実測, 予測) のペア列 |
| **PSO/PSO.h/.cpp** | 粒子群。bounds 付きコンストラクタ。setEvalData(i, eval_data), calcPersonalScore, updatePersonalBest, updateGlobalBest |
| **DE/DE.h/.cpp** | 差分進化。F, CR, bounds。setEvalData(i, eval_data), calcScore, mutation, selection |
| **LM/LM.h/.cpp** | setEvalData, setJacobian(d, col); getNextDelta() で (J^T J + λI)δ = -J^T r を解く。setLambda/getLambda で adaptive。gjm で連立一次方程式を解く（ピボットなし） |

### util/

| ファイル | 概要 |
|----------|------|
| **RunConfig.h** | trace_enabled, optimizer_names, n_iter_*, lm_apply_bounds |
| **TraceConfig.h/.cpp** | load(path) で trace / optimizer / lm_apply_bounds を読む。loadFromStruct(RunConfig)。isTraceEnabled, isLmApplyBoundsEnabled, getOptimizersToRun |
| **IResultWriter.h** | writeApplyOnly(fullParams, results), writeAfterOptimization(fullParams, results) |
| **OptimizerDriver.h/.cpp** | run(...) で PSO/DE/LM を分岐。runPSO/runDE/runLM 内で Objective, mapper の bounds/initial を使用。runLM は adaptive lambda + ステップ採用/却下 + bounds クリップ。runApplyOnly(mapper, model, loader, products, dbProvider, resultWriter) |
| **Handler.h/.cpp** | 設定ファイルを読んで getOptimizersToRun() |
| **IterationLog.h/.cpp** | 反復ログ用 |
| **csvwriter.hpp** | CSV 出力ヘルパ（必要に応じて） |

### mock/

| ファイル | 概要 |
|----------|------|
| **Demo.h/.cpp** | DemoPhysicalModel(1,2,3), DemoDataLoader(1,2,3)。quadratic: p0+p1*x+p2*x^2; linear: p0+p1*x; rational_exp: p0+p1/(1+x)+p2*exp(-x/5)。measured を真値から生成 |
| **demo_main.cpp** | makeSpecs() で p0,p1,p2 の初期値・bounds。Handler → 3 モデル × 最適化器で OptimizerDriver::run → result/summary.csv |
| **Mock.h/.cpp** | テスト用モデル・ローダ |
| **ResultWriterStub.h/.cpp** | IResultWriter スタブ |

---

## 3. 呼び出しの流れ（エントリ → 評価）

```
demo_main
  → Handler(config) → getOptimizersToRun()
  → OptimizerDriver::run(configPath, mapper, model, loader, products, optimizerName, ...)
       → TraceConfig::load(configPath)
       → runPSO / runDE / runLM のいずれか
            → Objective::evaluate(x) または evaluateWithJacobian(x)
                 → mapper.expandToFullParameterSet(x)
                 → BatchEvaluationHandler::evaluate(fullParams)
                      → 各 ProductMeta に対して ProductRunner::run(meta, fullParams)
                           → loader.load(meta) → ProductLoadedData
                           → model.run(fullParams, data) → predicted
                           → residuals = measured - predicted → ProductResidualBlock
                      → 全ブロックの residuals を連結 → ResidualAssembly
                 → objective = sqrt(sum(r^2)) など
            → アルゴリズムごとの更新（粒子/個体/パラメータ）
       → RunResult { bestScore, bestParams }
  → summary.csv 出力
```

- **runApplyOnly** の流れ：mapper.getInitialVector(dbValueProvider) で fullParams を組み、各製品を 1 回ずつ計算して ProductRunResult の列を作り、resultWriter.writeApplyOnly(fullParams, results) を呼ぶ。

---

## 4. 設定ファイルとの対応

| 設定 | 役割 | 読むコード |
|------|------|------------|
| **config/params_sample.csv** | パラメータ名・enable_opt・初期値・上下限・apply_bounds | CsvParamLoader, ParameterMapper |
| **config/para.cfg** | trace, optimizer, n_iter_pso/de/lm, lm_apply_bounds, pso_demo_mode | TraceConfig::load |

---

## 5. プログラマが特に見るべき点（チェックリスト）

- [ ] **残差の符号**：どこでも「measured - predicted」で統一されているか
- [ ] **パラメータ並び**：ParamSpec の並び ≒ fullParams の並び ≒ モデルの run(fullParams, ...) の期待並び
- [ ] **LM の eval_data**：first=実測, second=予測 で getNextDelta の右辺が -J^T r になっているか
- [ ] **bounds**：PSO/DE はコンストラクタで範囲を渡してクリップ。LM は runLM 内で TraceConfig::isLmApplyBoundsEnabled と getApplyBounds を見てクリップ
- [ ] **初期値**：getInitialVector(nullptr) が CSV の init_value / init_mode から正しく組み立てられているか
- [ ] **USERWORK**：IPhysicalModel, IProductDataLoader, DB 値取得、出力（IResultWriter）の差し替え箇所がコードと docs/USERWORK.md で一致しているか
- [ ] **数値**：LM の λ の上下限、最大リトライ数、Objective の ε、gjm の 0 割対策の有無

---

## 6. ファイル一覧（パスだけ・検索用）

```
core/core.hpp
model/IPhysicalModel.hpp
model/IProductDataLoader.hpp
product/ProductMeta.hpp
product/ProductLoadedData.hpp
product/ProductRunResult.hpp
product/ProductResidualBlock.hpp
product/ResidualAssembly.hpp
product/ProductRunner.hpp
product/ProductRunner.cpp
product/BatchEvaluationHandler.hpp
product/BatchEvaluationHandler.cpp
param/param.hpp
param/ParamSpec.cpp
param/CsvParamLoader.cpp
param/ParameterMapper.cpp
objective/Objective.h
objective/Objective.cpp
Optimizer/Optimizer.h
Optimizer/Optimizer.cpp
Optimizer/PSO/PSO.h
Optimizer/PSO/PSO.cpp
Optimizer/DE/DE.h
Optimizer/DE/DE.cpp
Optimizer/LM/LM.h
Optimizer/LM/LM.cpp
util/RunConfig.h
util/TraceConfig.h
util/TraceConfig.cpp
util/IResultWriter.h
util/OptimizerDriver.h
util/OptimizerDriver.cpp
util/Handler.h
util/Handler.cpp
util/IterationLog.h
util/IterationLog.cpp
mock/Demo.h
mock/Demo.cpp
mock/demo_main.cpp
mock/Mock.h
mock/Mock.cpp
mock/ResultWriterStub.h
mock/ResultWriterStub.cpp
```

以上で、Phase 1 → 2 → 3 → 4 の順に読めば、依存関係を崩さずに全体を把握できます。不明な箇所は README.md と docs/USERWORK.md もあわせて参照してください。
