# オンサイト組み込み向け リファクタリング提案

現場に「単体で動作する物理モデルシミュレータ」と「データローダー一式」がある前提で、本リポジトリ（para）を**容易に組み合わせられる形**にするためのリファクタリング案です。

---

## 1. 現状の整理

### 1.1 現場が触る境界（良い点）

| 境界 | 内容 | 現場の作業 |
|------|------|------------|
| **IPhysicalModel** | `run(fullParams, productLoadedData)` → 予測ベクトル | 既存シミュレータを薄いラッパで包む |
| **IProductDataLoader** | `load(ProductMeta)` → ProductLoadedData | 既存データ読込を薄いラッパで包む |
| **ProductLoadedData** | measured, positions（同長） | common 等から変換して詰める |
| **ParameterMapper / ParamSpec** | パラメータ並び・上下限・初期値 | 現場仕様の CSV/設定から構築 |
| **DbValueProvider** | db_key → 初期値 | 現場 DB から取得する関数を渡す |

→ **インターフェースはシンプルで、現場の「既存シミュレータ」「既存データローダ」を 1:1 でラップすればよい**設計になっている。

### 1.2 現状の課題（組み込みを重くしている点）

| 課題 | 内容 |
|------|------|
| **エントリがデモに一体化** | 最適化の「入り口」が demo_main にあり、PSO/DE/LM/Handler を直接 include してループを回している。現場が自前 main から「1 回だけ最適化して結果が欲しい」としたい場合、demo の内部を真似するか、同様のループを自前で書く必要がある。 |
| **ParameterMapper の二重管理** | runOne / runPSO / runDE / runLM のそれぞれで `makeSpecs()` により ParameterMapper を再構築している。現場が specs を 1 か所（CSV 等）で管理しても、para 側で同じ specs が複数箇所に散らばっていると、変更漏れや不一致が起きやすい。 |
| **設定がファイル固定** | Handler が config ファイルパスのみ受け取り、中身は para の key=value 形式に依存。現場が既存の設定系（別フォーマット・別ファイル）を持っている場合、そのまま渡せない。 |
| **「最適化 1 回」の API が無い** | 現場の main から「モデル・ローダ・製品リスト・設定を渡したら、指定した最適化器で 1 本走らせて結果を返してほしい」という 1 関数が無い。 |

---

## 2. リファクタリング提案（優先度順）

### 2.1 【高】「最適化ドライバ」API を 1 か所にまとめる

**目的**: 現場の main は「ドライバに model / loader / 設定 / 製品リストを渡して 1 回呼ぶ」だけにし、PSO/DE/LM の詳細を触らせない。

**案**:

- **新規** `OptimizerDriver`（または Handler の拡張）を用意する。
- **責務**:
  - 設定（ファイルパス or 構造体）の読込
  - 指定された最適化器名（PSO/DE/LM）に応じた 1 回分の実行
  - 入出力: **入力** = 設定パス（または RunConfig 構造体）、ParameterMapper（1 本）、IPhysicalModel&、IProductDataLoader&、製品リスト（vector&lt;ProductMeta&gt;）、**出力** = 最良スコア・最良パラメータ（および必要ならトレース用バッファ）

**呼び出しイメージ（現場の main）**:

```cpp
// 現場: 自前のモデル・ローダ・specs を準備
MyPhysicalModel model;      // IPhysicalModel を実装、中で既存シミュレータを呼ぶ
MyDataLoader loader;       // IProductDataLoader を実装、中で既存データ読込を呼ぶ
ParameterMapper mapper;
mapper.setSpecs(mySpecs);   // 現場の CSV/設定から 1 回だけ構築

OptimizerDriver driver;
auto result = driver.run("config/para.cfg", mapper, model, loader, productList);
// result.bestScore, result.bestParams を使用
```

- **効果**: 現場は PSO/DE/LM のヘッダを include せず、`OptimizerDriver`（と必要最小限の core / param / product / model）だけ見ればよい。組み込み時の変更箇所が少ない。

---

### 2.2 【高】ParameterMapper を 1 本化する

**目的**: specs と「初期ベクトル・上下限」の出所を 1 つにし、現場が 1 か所で管理できるようにする。

**現状**:  
runPSO / runDE / runLM の内部で、それぞれ `ParameterMapper mapper; mapper.setSpecs(makeSpecs());` のように自前で mapper を組み、`getLowerBounds()` / `getUpperBounds()` / `getInitialVector()` を取得している。一方、Objective を組み立てる runOne / runLMOne 側でも別の mapper で makeSpecs() を呼んでいる。

**案**:

- 最適化実行（runPSO / runDE / runLM 相当）の**呼び出し側**が、**1 本の ParameterMapper**（specs を 1 回 setSpecs したもの）を用意する。
- ドライバ（または runOne の枠）は、その **同じ mapper** を Objective 構築と、各最適化器への「初期点・上下限」の両方に渡す。
- runPSO / runDE / runLM は、**ParameterMapper の参照**（または初期ベクトル・下限・上限の 3 つ）を引数で受け取り、内部で makeSpecs() を呼ばない。

**効果**: 現場は「specs を 1 か所（CSV 等）で定義 → 1 本の mapper をドライバに渡す」だけで済む。para 内で specs が複数箇所にコピーされない。

---

### 2.3 【中】設定の注入方法を「ファイル or 構造体」にする

**目的**: 現場が既存の設定ファイル・設定システムを持っている場合、para の key=value ファイルに合わせなくても渡せるようにする。

**案**:

- **RunConfig** のような構造体を用意する。
  - 例: `trace_enabled`, `optimizer_names` (vector&lt;string&gt;), 必要なら反復数など。
- Handler（または TraceConfig）は、
  - **従来**: ファイルパスから load して TraceConfig の静的変数を埋める。
  - **拡張**: `void loadFromStruct(const RunConfig&)` のような API を追加し、ファイルを読まずに RunConfig の内容で上書きできるようにする。
- ドライバの `run()` は、**設定**として「ファイルパス」または「RunConfig」のどちらかを受け付ける。

**効果**: 現場が「自前の設定を RunConfig に詰めて渡す」だけで、para の設定ファイル形式に縛られない。

---

### 2.4 【中】デモは「ドライバの利用例」に寄せる

**目的**: 本番組み込みとデモの差を「モデル・ローダ・specs の差」だけにし、ループ構造を 1 本にする。

**案**:

- demo_main は、
  - makeSpecs() は **1 回だけ** 呼び、**1 本の ParameterMapper** を構築。
  - デモ用の IPhysicalModel / IProductDataLoader の実装（既存の 3 モデル・3 ローダ）と、製品リストを準備。
  - **OptimizerDriver::run(...)** を呼ぶ（または「モデル×最適化器」の二重ループはドライバの外で回し、1 モデル・1 最適化器ごとに run を 1 回呼ぶ）。
- runPSO / runDE / runLM の実体は、**ドライバの .cpp か、ドライバからだけ呼ばれる 1 か所**にまとめ、demo_main からは直接呼ばない。

**効果**: 現場の main とデモの main が「同じドライバ API を呼ぶ」形で揃い、説明と保守がしやすい。

---

### 2.5 【低】ProductMeta の拡張性（任意）※未実装

**目的**: 現場が「コイル NO」「日付」など、product_id / file_path 以外のキーを持っている場合に、無理なく渡せるようにする。

**案**:

- ProductMeta に、`std::map<std::string, std::string> extra` のような任意キーを 1 つ持たせる（または optional なフィールドを数個追加）。
- IProductDataLoader::load(ProductMeta) の実装側で、extra や追加フィールドからファイルパス・コイル NO などを取り出す。

**効果**: 既存の product_id / file_path はそのままにしつつ、現場固有のメタ情報を 1 つの型で扱える。

**現状**: 未実装。テスト環境（CppUTest のメモリ制約）との兼ね合いで extra は見送り。現場では product_id / file_path にまとめるか、必要に応じて ProductMeta を拡張して利用する。

---

### 2.6 【低】IPhysicalModel::run の第二引数型（現状維持を推奨）

**目的**: 現場の「既存シミュレータ」が既に「自分のデータ構造」で動いている場合のラップのしやすさを保つ。

**現状**: `run(fullParams, const void* productLoadedData)` で、実装側で ProductLoadedData* にキャストして使う想定。

**提案**: リファクタでは**シグネチャは変えず、ドキュメントだけ明確化**する。

- USERWORK や ONSITE 用ドキュメントに「通常は ProductLoadedData* にキャスト。現場で別型を渡す場合は、ラッパ内で現場型 → 予測用入力に変換してから既存シミュレータを呼ぶ」と記載する。
- これにより、**現場の既存コードをなるべくそのまま使い、ラッパの厚さを最小**にできる。

---

## 3. 現場の「単体シミュレータ・データローダ」との接続イメージ（リファクタ後）

```
[現場: 既存の物理モデルシミュレータ]   →  IPhysicalModel を実装した薄いラッパが 1 回呼ぶ
[現場: 既存のデータローダ一式]         →  IProductDataLoader を実装した薄いラッパが load() で ProductLoadedData を返す
[現場: パラメータ定義・製品リスト]     →  ParameterMapper + vector<ProductMeta> を 1 回組み、ドライバに渡す
[現場: 設定（任意）]                   →  ファイルパス or RunConfig をドライバに渡す

para: OptimizerDriver::run(config, mapper, model, loader, products)
      → 内部で Objective / Handler / PSO or DE or LM を組み、結果を返す
```

- 現場の main は **「model / loader / mapper / products / config を組み、driver.run() を 1 回（または製品・最適化器のループで複数回）呼ぶ」** だけになる。
- 現場は **IPhysicalModel, IProductDataLoader, ProductLoadedData, ProductMeta, ParameterMapper, RunConfig, OptimizerDriver** の利用方法だけ理解すればよく、PSO/DE/LM の内部には触れない。

---

## 4. 実装の優先順位（目安）

| 順位 | 項目 | 理由 |
|------|------|------|
| 1 | 2.1 最適化ドライバ API | 現場のエントリを 1 関数にまとめ、組み込み時の変更を最小にするため。 |
| 2 | 2.2 ParameterMapper の 1 本化 | 設定の一貫性と現場の 1 か所管理のため。 |
| 3 | 2.4 デモをドライバ利用に | デモと本番の差を「データとモデル」だけにし、説明を簡単にするため。 |
| 4 | 2.3 設定の注入（RunConfig） | 現場の既存設定との併存をしやすくするため。 |
| 5 | 2.5 ProductMeta 拡張 / 2.6 ドキュメント | 必要に応じて。 |

---

## 5. まとめ

- **インターフェース（IPhysicalModel, IProductDataLoader, ProductLoadedData, ParameterMapper）は、現場の単体シミュレータ・データローダと 1:1 でラップしやすい形になっている。**
- リファクタの中心は **「最適化 1 回分の入り口」を 1 か所にまとめること** と **「ParameterMapper を 1 本にすること」**。これで現場は「自前の model / loader / specs / products を用意してドライバを呼ぶ」だけで、para と組み合わせられる。
- 設定は **ファイル or 構造体** の両方を受け付けるようにすると、現場の既存設定と組み合わせやすい。

この方針で進めれば、オンサイトに持っていった際に**現場プログラムと容易に組み合わせられる**状態にできます。
