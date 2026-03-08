# リファクタリング提案

現状のコードベースを踏まえた改善案です。優先度・スコープは要検討です。

---

## 1. ResultOutput の整理

### 1.1 役割の分離

- [x]**実施**
- **現状**: PLOG 用の `plogHeaders_` / `plogRows_` と、汎用の `headers_` / `rows_` / `currentRow_` が混在している。
- **提案**: PLOG 専用の小さなクラス（例: `ProductLogBuffer`）に分離し、`ResultOutput` は「汎用 CSV 表の addColumn / endRow / flush」と LLOG/DLOG のみ担当する。PLOG は「1 製品 1 行・列を横に追加・終了時 flush」に特化した型にすると見通しがよい。

### 1.2 Before/After の扱い

- [x] **実施**
- **現状**: LLOG/DLOG は `ResultTiming` でファイル名や書き出しタイミングを切り替えている。PLOG は終了時 1 回 flush に変更済み。
- **提案**: 「前・後」を区別しないなら、LLOG/DLOG の `ResultTiming` 引数をやめ、ファイル名は 1 種類（例: 設定で 1 本指定）にしてもよい。必要なら呼び出し側でファイル名を変える。

前後の区別は不要

### 1.3 未使用 API の整理

- [x] **実施**
- **setFilenameSame** は PLOG を「終了時 flush」にしたあと、主にサンプルや他ログ用。PLOG は `setPLOGFilename` のみ使う。
- **提案**: setFilenameSame を「汎用の 2 タイミング書き出し」用としてコメントで用途を明示するか、使っていなければ削除を検討。

---

## 2. TraceConfig の整理

### 2.1 設定のグループ化

- [ ] **実施**
- **現状**: 静的メンバが多く、キー解析が 1 つの `load()` に集中している。
- **提案**:
  - 設定を「トレース/デバッグ」「最適化器」「結果出力」「データフィルタ」などにグループ化し、読込処理を関数分割（例: `loadTraceSection()`, `loadResultSection()`）する。
  - あるいは、キーごとのハンドラをテーブル化（キー → 設定関数）して `load()` を短くする。

### 2.2 result_* と PLOG の対応

- [ ] **実施**
- **現状**: `result_filename_before` が PLOG の出力先としても使われている。
- **提案**: 設定キーを `result_plog_filename` のように PLOG 用に明示するか、コメントで「PLOG の出力先に result_filename_before を使用」と明記する。

---

## 3. 命名・用語の統一

- [x] **実施**
- **ResultTiming**: PLOG では使わなくなったが、LLOG/DLOG ではまだ使用。ドキュメントとコードで「前/後」が必要かどうかを揃える。
- **RO::Before / RO::After**: 汎用 CSV の 2 回書き出し用として残すか、廃止して呼び出し側でファイル名を渡すだけにするかを決める。
- **config/para.cfg**: キー名に `result_` が多く、どのキーが PLOG/LLOG/DLOG に対応するか一覧表があるとよい（README や本提案の末尾表に追記）。

キー名自体をLOGに合わせ変するｈｓ

---

## 4. オンサイト (main_onsite.cpp) の整理

- [ ] **実施**
- **現状**: makeSpecs / makeProducts / Model / Loader / ResultWriter が 1 ファイルにまとまっている。
- **提案**:
  - 可能なら「設定・製品リスト」「モデル・ローダ」「結果出力」を別 .cpp に分割し、オンサイト用の最小ライブラリとしてリンクする構成を検討する。
  - あるいは、現状の 1 ファイルのまま、セクションコメント（1. makeSpecs 等）と PLOG/LLOG/DLOG の役割を先頭で簡潔に説明するだけにとどめる。

---

## 5. テスト・ビルド

- [x] **実施**
- **ResultOutput**: PLOG の「列を横に追加」「product_id でマッチ」をユニットテストで担保すると安全。
- **TraceConfig**: `optimization_data_types` / `optimization_position_min|max` の読込と getter のテストがあるとよい。
- **Makefile**: 追加した util（ResultOutput 等）が test ターゲットでリンクされているか確認する。

---

## 6. ドキュメント

- [x] **実施**
- **CODE_READING_GUIDE.md**: ResultOutput の PLOG/LLOG/DLOG と「終了時 flush」を追記する。
- **USERWORK.md**: IResultWriter と ResultOutput（PLOG/LLOG/DLOG）の関係を 1 文で補足する。
- **OPTIMIZER_PARAMS.md**: para.cfg の result_* / optimization_data_types / optimization_position_* を一覧に含める。
- 不要になった文書（古い構成案・重複する設定説明）は削除し、上記に集約する。

---

## 7. 設定キーと PLOG/LLOG/DLOG の対応（案）

- [x] **実施**（README / para.cfg コメントへの反映）

| 用途       | 設定キー例                          | 備考                    |
|------------|-------------------------------------|-------------------------|
| PLOG 出力  | result_filename_before（流用）      | 終了時 1 回 flush       |
| LLOG 出力  | result_detail_filename              | 全製品 1 ファイル       |
| DLOG 出力  | result_detail_filename_per_product  | 1 製品 1 ファイル      |
| 詳細の有無 | result_detail_enabled               | LLOG/DLOG の有効/無効   |
| 範囲       | result_detail_start_index, max_points | 点の範囲              |

LOGに合わせてキーやシンボルは名前変更する。
