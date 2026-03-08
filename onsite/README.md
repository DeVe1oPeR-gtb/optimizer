# オンサイト用メイン

現場で編集するのは **main_onsite.cpp** のみ。このディレクトリと Makefile の設定はバッチで一括して行う。

## プロジェクトルート

**setup を置いた場所（`scripts/` の親ディレクトリ）がプロジェクトルート**です。setup 実行のたびに、ルートを基準に必要なディレクトリ（`onsite/`, `config/`, `log/`, `result/`）を配置し直します。

## 手順

1. **バッチを実行する**（プロジェクトルートで）
   ```bash
   bash scripts/setup_onsite.sh
   ```
   - ルート基準で `onsite/`, `config/`, `log/`, `result/` を用意し、Makefile に `onsite` ターゲットを追加する。
   - `onsite/main_onsite.cpp` が無い場合はテンプレートからコピーする。
   - 既に Makefile に onsite が入っている場合は追記しない。

2. **main_onsite.cpp を編集する**
   - `makeSpecs()`: パラメータ仕様（CSV や現場設定から組み立て）
   - `makeProducts()`: 製品メタのリスト
   - `OnsiteModel`: `IPhysicalModel` の実装（現場の物理モデルまたは compat_model）
   - `OnsiteLoader`: `IProductDataLoader` の実装（現場のデータ読込または compat_data）
   - 必要なら `IResultWriter` を実装し、`OptimizerDriver::run(..., resultWriter)` に渡す。

3. **ビルド・実行**
   ```bash
   make onsite
   ```
   - ビルド後、そのまま実行される。

## 設定

- 設定ファイル: `config/para.cfg`（demo と同じ。未配置の場合は `RunConfig` を直接組み立てて `OptimizerDriver::run(config, ...)` を呼ぶ）。
- 実行ファイル: `build/Onsite`

## 新しいプロジェクトを現場用に1本化する

現場ルートに既にある **物理モデル用プロジェクト（src/sim）** と、**optimizer の必要な部分だけ**をコピーして、1つの新規プロジェクトとして組み立てる場合:

```bash
bash scripts/setup_new_onsite_project.sh --field-root /path/to/既存の現場プロジェクト --target /path/to/新規プロジェクト
```

- **field-root**: `src/` と `sim/` がある既存のオンサイト用プロジェクトのパス。
- **target**: 作成する新規プロジェクトのパス（無ければ作成）。
- optimizer からは **mock・tests・デモはコピーせず**、Optimizer / param / product / objective / model / core / util / onsite / config のみコピーする。
- 新規プロジェクトには Makefile が生成され、`make onsite` で `build/Onsite` をビルド・実行できる。`main` は `onsite/main_onsite.cpp` のみ。

## 参考

- 差し替え箇所一覧: リポジトリルートの `docs/USERWORK.md`
- 構成案: `docs/ONSITE_LAYOUT_PROPOSAL.md`
