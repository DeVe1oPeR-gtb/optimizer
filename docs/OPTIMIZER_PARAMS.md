# 最適化器の主要パラメータ一覧

以下は PSO / DE / LM の主要パラメータです。**いずれも `config/para.cfg` で指定可能**です（TraceConfig が読込、OptimizerDriver が参照）。

---

## PSO（粒子群最適化）

| パラメータ | 意味 | 現在の設定箇所 | 典型値・備考 |
|------------|------|----------------|--------------|
| **w** | 慣性重み (inertia) | `OptimizerDriver.cpp` L79: `w(N_DIM, 0.7)` / test_PSO DEMO: 0.7～0.8 | 0.7。参照サンプルでは反復ごとに 0.7～0.8 の乱数 |
| **c1** | パーソナルベスト重み | `OptimizerDriver.cpp` L79: `c1(N_DIM, 1.8)` / test: 2.0 | 1.8 または 2.0 |
| **c2** | グローバルベスト重み | `OptimizerDriver.cpp` L79: `c2(N_DIM, 1.8)` / test・参照: 0.8 | 1.8（ドライバ）または 0.8（参照） |
| **粒子数** | 群サイズ | `OptimizerDriver.cpp` L30: `N_PARTICLE = 20` | 20。DEMO では 500 |
| **初期範囲** | 粒子の初期化範囲 | `OptimizerDriver.cpp` L81-84: `z0[i] ± 0.5` | 初期値の前後 0.5 |

- コンストラクタ: `PSO(num_particles, num_dim, w, c1, c2, ul_bound, ll_bound, seed)`
- 境界 `ul_bound` / `ll_bound` は `ParameterMapper` から取得。初期範囲だけドライバで `z0 ± 0.5` にしている。

---

## DE（差分進化）

| パラメータ | 意味 | 現在の設定箇所 | 典型値・備考 |
|------------|------|----------------|--------------|
| **F** | 変異係数 (scale factor) | `OptimizerDriver.cpp` L147: `0.5` / test_DE: 0.5 | 0.5。通常 0.4～1.0 |
| **CR** | 交叉率 (crossover rate) | `OptimizerDriver.cpp` L147: `0.9` / test_DE: 0.9 | 0.9。通常 0.8～1.0 |
| **個体数** | 集団サイズ | `OptimizerDriver.cpp` L31: `N_POP_DE = 20` | 20 |
| **初期範囲** | 個体の初期化範囲 | `OptimizerDriver.cpp` L149-151: `z0[i] ± 0.5` | 初期値の前後 0.5 |

- コンストラクタ: `DE(pop_size, num_dim, F, CR, ul_bound, ll_bound)`
- 変異式: `mutant[d] = population[a][d] + F * (population[b][d] - population[c][d])`（DE/rand/1 相当）

---

## LM（Levenberg–Marquardt）

| パラメータ | 意味 | 現在の設定箇所 | 典型値・備考 |
|------------|------|----------------|--------------|
| **lambda** | ダンピングパラメータ λ | `LM.cpp` L18: `lambda_ = 10.0`（初期） | 10.0。adaptive で反復中に更新 |
| **r** (摂動量) | ステップ幅計算用の摂動 | `LM.cpp` L19: `r_.resize(..., 0.005)` | 0.005。`delta_[d] = z[d]*r_[d]` で初期化 |
| **lambda_min** | λ の下限 | `OptimizerDriver.cpp` L220: `1e-10` | 1e-10 |
| **lambda_max** | λ の上限 | `OptimizerDriver.cpp` L221: `1e12` | 1e12 |
| **lambda_down** | ステップ採用時の λ 縮小率 | `OptimizerDriver.cpp` L259: `lm.getLambda() * 0.5` | 0.5 |
| **lambda_up** | ステップ却下時の λ 拡大率 | `OptimizerDriver.cpp` L262: `lm.getLambda() * 10.0` | 10.0 |
| **lm_max_try** | 1 反復あたりのステップ試行回数 | `OptimizerDriver.cpp` L222: `8` | 8 |

- コンストラクタ: `LM(num_dim, num_data, z)`。内部で `lambda_ = 10.0`、`r_[d] = 0.005` を固定。
- `setLambda()` で反復中に λ を更新。bounds は `TraceConfig::isLmApplyBoundsEnabled()` と `mapper.getApplyBounds()` で適用有無を制御。

---

## ドライバ共通（OptimizerDriver）

| パラメータ | 意味 | 現在の設定箇所 |
|------------|------|----------------|
| **n_iter_pso** | PSO 反復数 | `para.cfg` → `TraceConfig`（既定 120） |
| **n_iter_de** | DE 反復数 | `para.cfg` → `TraceConfig`（既定 120） |
| **n_iter_lm** | LM 反復数 | `para.cfg` → `TraceConfig`（既定 80） |
| **N_PARTICLE** | PSO 粒子数 | `OptimizerDriver.cpp` L30: 20（固定） |
| **N_POP_DE** | DE 個体数 | `OptimizerDriver.cpp` L31: 20（固定） |
| **初期範囲オフセット** | PSO/DE の初期化 `z0 ± value` | L83-84, L150-151: 0.5（固定） |

---

## para.cfg のキー一覧（実装済み）

| キー | 意味 | 既定値 |
|------|------|--------|
| trace | トレース出力の有無（on/off） | on |
| **debug** | **デバッグログの有無（on/off）。trace とは別。on のとき run 開始前に必要な入力のチェック結果をログ出力。** | off |
| optimizer | 実行する最適化器（カンマ区切り） | PSO,DE,LM |
| lm_apply_bounds | LM でパラメータ上下限を適用するか（on/off） | on |
| n_iter_pso / n_iter_de / n_iter_lm | 各最適化器の反復数 | 120 / 120 / 80 |
| **PSO** | | |
| pso_w | 慣性重み | 0.7 |
| pso_c1 | パーソナルベスト重み | 1.8 |
| pso_c2 | グローバルベスト重み | 1.8 |
| pso_n_particle | 粒子数 | 20 |
| pso_init_radius | 初期範囲の半径（z0±） | 0.5 |
| **DE** | | |
| de_f | 変異係数 | 0.5 |
| de_cr | 交叉率 | 0.9 |
| de_n_pop | 個体数 | 20 |
| de_init_radius | 初期範囲の半径 | 0.5 |
| **LM** | | |
| lm_lambda_init | 初期ダンピング λ | 10.0 |
| lm_r_perturb | 摂動量 | 0.005 |
| lm_lambda_min / lm_lambda_max | λ の下限/上限 | 1e-10 / 1e12 |
| lm_lambda_down / lm_lambda_up | ステップ採用時の λ 縮小率/却下時の拡大率 | 0.5 / 10.0 |
| lm_max_try | 1 反復あたりの最大試行回数（1 未満は 1 に clamp） | 8 |
| **結果出力（PLOG / LLOG / DLOG）** | | |
| plog_filename | PLOG 出力先（1 製品 1 行・列横追加。終了時 1 回 flush）。{timestamp}, {product_id} 利用可 | （空なら無効） |
| csv_filename_after | 汎用 CSV 最適化後用（2 タイミング書き出し時） | （空なら無効） |
| detail_enabled | LLOG/DLOG を有効にするか（1/on/true） | 0 |
| detail_start_index / detail_max_points | 詳細の開始インデックス・最大点数 | 0 / 0 |
| llog_one_file | LLOG を全製品 1 ファイルにまとめるか | 1 |
| llog_filename | LLOG 出力先（全製品 1 ファイル） | （空なら無効） |
| dlog_filename | DLOG 出力先（1 製品 1 ファイル）。{product_id} 利用可 | （空なら無効） |
| result_file_max_bytes / result_total_max_bytes | 1 ファイル・合計の最大バイト（0=制限なし） | 0 / 0 |
| result_final_params_filename | 最適化終了パラメータを書き出すテキストファイル | （空なら無効） |
| **データフィルタ** | | |
| optimization_data_types | 最適化に使う data_type_id（カンマ区切り。空なら全種類） | （空） |
| optimization_position_min / optimization_position_max | position の有効範囲（0～1）。範囲外は最適化対象外 | 0.0 / 1.0 |
