# 現場組み込み用 構成提案

本プロジェクト（optimizer）を `para` に配置し、同階層に `sim` と `src` を置く前提の構成案です。現地作業で組み込みしやすいように境界と役割を明確にしています。

---

## 1. トップレベル構成

```
（現場ルート）
├── para/          # 本プロジェクト（最適化ライブラリ）をそのまま配置
├── sim/           # シミュレーションメイン + オンラインソース互換レイヤ
├── src/           # 実際の物理モデル（オンラインソース）
（コモンは sim 内の common_glb.hxx に定義）
```

- **コモン（DB バイナリをロードする先の構造体）** は **sim のヘッダ `common_glb.hxx` に定義済み**。sim と src はこのヘッダを include してグローバル構造体を参照する。

---

## 2. 各ディレクトリの役割

| ディレクトリ | 役割 | 備考 |
|-------------|------|------|
| **para** | 最適化（Optimizer/ の下に PSO/, LM/, DE/）、ParameterMapper、Objective、製品バッチ評価の枠組み | 本リポジトリをそのままコピー。物理モデル・DB読込は呼び出し側に委譲。 |
| **sim** | シミュレーションのメイン処理と、オンラインソースを「シミュレータ／最適化から呼べる形」に合わせるレイヤ | メインエントリ、DB バイナリ読込、common へのロード、物理モデル呼び出しのラップ。 |
| **src** | 実際の物理モデル（オンラインソース） | グローバル変数（common の構造体）を参照して計算。そのままオンラインでも利用。 |
| **common（コモン）** | 構造体定義とグローバル変数（DB バイナリをロードする先） | **sim のヘッダ `common_glb.hxx` に定義済み**。DB バイナリをここにロードし、src と sim の両方が参照。 |

---

## 3. データフローと責務

```
[DB バイナリファイル]
  コイルNO(6桁英数)_yyyymmdd.構造体名
  ↓ ロード（sim で実装）
[common のグローバル構造体]
  ↓ 参照
[src: 物理モデル]  ← グローバルを読んで計算
  ↓ 呼び出し（sim の互換レイヤ経由）
[para: ProductRunner / Objective]
  ← 最適化ベクトル x を common に反映してからモデル実行、残差を返す
```

- **DB バイナリ**: `sim` がファイル名規則に従ってオープンし、common の構造体へ読み込む。
- **物理モデル**: `src` は common のグローバルのみ参照。入出力は「common 経由」。
- **互換レイヤ（sim）**:  
  - common に「現在の最適化パラメータ」を書き込む  
  - 物理モデル（src）を呼ぶ  
  - 結果を common または戻り値から取り、para の求める形式（例: 予測ベクトル or 残差）に変換して返す。

---

## 4. DB バイナリのファイル名規則

- **形式**: `{コイルNO(6桁英数)}_{年月日(yyyymmdd)}.{構造体名}`  
- **例**: `COIL01_20250307.MeasureData`  
- **解釈**:  
  - コイルNO + 日付で「どの製品のいつ時点のデータか」を特定。  
  - `.{構造体名}` で、どの common 構造体にロードするかを判定（現場の命名に合わせる）。

**sim 側で行うこと（提案）**  
- 製品メタ（例: product_id, 日付）から上記ファイル名を組み立てる。  
- ファイルをオープンし、対応する common の構造体へバイナリ読込。  
- 読込ルーチンは現場のバイナリレイアウトに合わせて **sim** に 1 か所まとめる（para は「common が更新済み」という前提でだけ使う）。

---

## 5. common（コモンエリア）の扱い

- **定義**: 構造体・グローバル変数は **sim のヘッダ `common_glb.hxx` に定義済み**。  
- **参照**: sim と src は `#include "common_glb.hxx"` でコモンを参照。DB バイナリは sim の db_loader がこの構造体へロードする。  
- **para 側**: common を直接 include せず、sim の互換レイヤ（IPhysicalModel / IProductDataLoader 実装）経由でのみ利用する。

---

## 6. sim の推奨サブ構成

```
sim/
├── main_sim.cpp（または main.c）   # シミュレーションメイン（パラメータ掃引・ログ出力など）
├── db_loader.h / db_loader.cpp    # DB バイナリ → common 構造体へのロード（ファイル名規則の実装）
├── compat/                        # オンラインソースとシミュレータ互換
│   ├── compat_model.h / .cpp      # common にパラメータをセット → src のモデル呼び出し → 予測取得
│   └── compat_data.h / .cpp      # common から実測・位置などを取り、para の ProductLoadedData 等に変換
├── Makefile（または build スクリプト）
└── README_sim.md                  # 現地でのビルド手順・common のパス・ファイル名規則のメモ
```

- **db_loader**: ファイル名 `{コイルNO}_{yyyymmdd}.{構造体名}` を組み立て、バイナリを読んで common へ格納。  
- **compat_model**: para の `IPhysicalModel` を実装。内部で common に x を書き、src の物理モデルを 1 回呼び、予測を `std::vector<double>` で返す。  
- **compat_data**: para の `IProductDataLoader` 相当または ProductLoadedData 構築。common のロード済みデータから measured/positions を埋める。

---

## 7. para と sim/src の接続イメージ

- **実行時の流れ（例）**  
  1. シミュレーションメイン（sim）が起動。  
  2. 必要なら DB バイナリを db_loader で common にロード。  
  3. para の Objective を構築する際に、  
     - **物理モデル**: sim の `compat_model`（IPhysicalModel 実装）を渡す。  
     - **データローダ**: sim の `compat_data`（IProductDataLoader 相当）を渡す。  
  4. 最適化ループでは、para が `evaluate(x)` を呼ぶたびに、sim の互換レイヤが common に x を反映 → src のモデル実行 → 残差を para に返す。

- **ビルド**  
  - **para 単体**: 既存の Makefile のまま。common に依存しない。  
  - **sim + src + common**: 現場用の Makefile を sim に用意。common のインクルードパス・ライブラリパスを現地環境に合わせて指定。  
  - **統合実行ファイル（任意）**: sim の main から para の API を呼ぶ形にすれば、1 本の実行ファイルにまとめられます。

---

## 8. 現地作業で行うこと（チェックリスト）

| 番号 | 作業内容 | 主に触る場所 |
|------|----------|--------------|
| 1 | common_glb.hxx の構造体レイアウトに合わせて DB バイナリ読込を実装する | sim/db_loader |
| 2 | DB バイナリのファイル名規則とレイアウトに合わせて db_loader を実装 | sim/db_loader.* |
| 3 | コイルNO・日付・構造体名と「製品 ID」の対応を決め、ProductMeta とファイルパスを対応させる | sim/compat_data 周り |
| 4 | common に最適化パラメータを書き込む処理と、src の物理モデル呼び出しを実装 | sim/compat_model |
| 5 | common から実測・位置などを取り、ProductLoadedData や residual の並びに変換 | sim/compat_data |
| 6 | ParameterMapper の db_key と DB の対応（初期値の取得）を実装 | sim または para の DbValueProvider |
| 7 | 必要なら Objective の目的関数式・重みを現場仕様に変更 | para/objective/Objective.cpp |

---

## 9. USERWORK と sim/src の対応

| USERWORK（本リポジトリ） | 現地で実装する場所 |
|--------------------------|---------------------|
| 実物理モデル本体 | **src**（既存オンラインソース）。呼び出し口は **sim/compat_model**。 |
| 実製品データローダ | **sim/compat_data**。DB バイナリは db_loader で common へロード済みとし、common から ProductLoadedData を組み立てる。 |
| 実 DB 値ローダ（初期値用） | **sim** の db_loader または専用関数。db_key とコイルNO・構造体・要素の対応を決めて DbValueProvider に渡す。 |
| FullParameterSet の定義詳細 | **common** の構造体と para の ParameterMapper の対応表を **sim** の README または compat のコメントに記載。compat_model で x → common への写像を実装。 |
| 実製品フォーマット依存処理 | **sim/db_loader**（バイナリレイアウト）、**sim/compat_data**（測点並び・residual 並び）。 |

---

## 10. まとめ

- **para**: そのまま `para/` に配置。物理モデル・DB はインターフェース経由で受け取り、common に直接依存しない。  
- **sim**: シミュレーションメイン + DB バイナリ読込（ファイル名規則） + common との橋渡し + para の IPhysicalModel / IProductDataLoader の実装。  
- **src**: 既存の物理モデル（オンラインソース）。グローバル（common）のみ参照。  
- **common**: 現場用意のコモンエリア。DB バイナリは `コイルNO(6)_yyyymmdd.構造体名` で sim がロードする。  

この構成にしておけば、「common のパスとファイル名規則だけ現場で合わせる」「sim の互換レイヤだけ実装する」という最小の現地作業で、para + sim + src を組み合わせて動かしやすいです。

次のステップとして、この案に沿って **sim の骨格（ディレクトリ・スタブの db_loader / compat_model / compat_data）** を para リポジトリ内に `docs/` や `site_layout/` のような形でサンプルとして追加する実装も可能です。必要なら「実装まで進める」と指定してください。
