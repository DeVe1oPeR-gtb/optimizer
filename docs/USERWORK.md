# USERWORK 一覧

ユーザー本人が後で実装・差し替えすべき箇所です。コード内では `// USERWORK:` で検索できます。


| #   | 対象                     | 内容                                         | 入力/出力                                                           | ファイル目安                              |
| --- | ---------------------- | ------------------------------------------ | --------------------------------------------------------------- | ----------------------------------- |
| 1   | 実物理モデル本体               | 1製品分の入力から1製品分の予測ベクトルを返す計算に差し替え             | 入力: 全パラメータ・製品データ / 出力: 予測ベクトル                                   | IPhysicalModel 実装                   |
| 2   | 実製品データローダ              | 製品バイナリ/ファイルの読込と ProductLoadedData の構築      | 入力: ProductMeta (file_path 等) / 出力: ProductLoadedData           | IProductDataLoader 実装               |
| 3   | 実 DB 値ローダ              | init_mode=db / hybrid のとき、db_key に対応する値を返す | 入力: db_key / 出力: double                                         | ParameterMapper に渡す DbValueProvider |
| 4   | FullParameterSet の定義詳細 | 物理モデルが受け取る「全パラメータ」の型・並び・意味                 | モデルと ParameterMapper の橋渡し                                       | 製品仕様に合わせた構造定義                       |
| 5   | 実製品フォーマット依存処理          | ファイル形式・測点並びに依存する読込・residual 並び             | 上記 2 と連動                                                        | データローダ・ProductRunner                |
| 6   | 実運用向け objective の詳細    | 重み付け・評価式・異常値処理など                           | evaluate 内の objective 計算式                                       | objective/Objective.cpp             |
| 7   | **結果出力**               | 適用値計算・最適化終了後の結果をファイル等に書き出す                 | 入力: fullParams, ProductRunResult 列 / 出力: 現場仕様                   | IResultWriter 実装（オンサイトで作成）          |
| 8   | **対象外データ点**            | 製品ごとに最適化対象から外すデータ点（非定常部等）の決定               | ロジックはオンサイト。ProductMeta.excluded_data_indices に 0 始まりのインデックスをセット | 製品リスト構築時（main 等）                    |


## 差し替え方

- **物理モデル**: `IPhysicalModel` を実装したクラスを用意し、ProductRunner 構築時に渡す。`run(fullParams, productLoadedData)` の第二引数は通常 `ProductLoadedData`* にキャストして使用。現場で別型を渡す場合は、ラッパ内で現場型→予測用入力に変換してから既存シミュレータを呼ぶ。
- **データローダ**: `IProductDataLoader` を実装したクラスを用意し、ProductRunner 構築時に渡す。コイルNO・日付等は product_id / file_path にまとめるか、現場で ProductMeta を拡張して利用する。
- **対象外データ点**: 非定常部などを最適化から外す場合、製品リストの各 `ProductMeta` の `excluded_data_indices` に、対象外とするデータ点の番号（0 始まり）を詰める。空のときは全点を対象。どの点を外すかのロジックはオンサイトで実装する。
- **DB 初期値**: `ParameterMapper::getInitialVector(DbValueProvider)` に、db_key → 値 を返す関数を渡す。
- **目的関数式**: `Objective::evaluate` 内の objective 計算（現在は sqrt(sum of squares)）を重み付け・式に合わせて変更する。

## 現場組み込み時のエントリ

- **OptimizerDriver::run(...)** に、設定（ファイルパスまたは `RunConfig`）、**1 本の ParameterMapper**、model、loader、製品リスト、最適化器名を渡すと 1 回分の最適化を実行し `RunResult`（最良スコア・パラメータ）を返す。現場の main は PSO/DE/LM を include せず、この API だけ呼べばよい。
- 設定をファイルに依存させない場合は `RunConfig` に `trace_enabled`・`optimizer_names`・反復数を詰めて `OptimizerDriver::run(config, mapper, ...)` を呼ぶ。

## 結果出力（オンサイトで実装）

- **IResultWriter** を実装し、`writeApplyOnly` と `writeAfterOptimization` で現場仕様のファイル・DB 等に書き出す。
- **適用値のみ計算（最適化なし）**: `OptimizerDriver::runApplyOnly(mapper, model, loader, products, dbValueProvider, resultWriter)` を呼ぶ。DB から読んだ適用値と設定の初期値で `getInitialVector(dbValueProvider)` により fullParams を組み、各製品を 1 回ずつ計算して `resultWriter.writeApplyOnly(fullParams, results)` が呼ばれる。
- **最適化終了後**: `OptimizerDriver::run(..., resultWriter)` に `IResultWriter`* を渡すと、最適化終了後に最適化済みパラメータで各製品を再計算し、`resultWriter.writeAfterOptimization(fullParams, results)` が呼ばれる。
- 各呼び出しで渡る `ProductRunResult` は製品ごとの `product_id`・`measured`・`predicted`・`residuals`（と `ok` / `error_message`）。出力形式は IResultWriter 実装側で自由に決める。

