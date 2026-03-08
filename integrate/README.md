# para 統合ビルド

ONSITE（オンサイト）と optimizer をリンクして **para** を構築するためのディレクトリです。

## 手順

### 1. バッチを叩く（リポジトリルートで）

```bash
bash scripts/integrate_project.sh
```

- **初回**: 以下を入力する。
  - オンサイトの **ONSITE ディレクトリ**のフルパス（例: `/home/user/project/ONSITE`）
  - **optimizer ディレクトリ**のフルパス（省略時はこのリポジトリのルート）
- 入力後、`integrate/Makefile.para` が生成され、`integrate/.onsite_paths` にパスが保存される。

### 2. 構築済みのとき（2 回目以降）

同じく `bash scripts/integrate_project.sh` を実行する。

- 保存済みのパスで Makefile を再生成し、既存の `Makefile.para` と **差分**を表示する。
- 差分がなければ「差分はありません」で終了。
- 差分があるとき:
  - **OK - 差分を適用しますか? (y/n)** で `y` を入力すると、新しい内容で `Makefile.para` を上書きする。
  - 差分だけ見たい場合は `bash scripts/integrate_project.sh --diff-only` で適用せず表示のみ。

### 3. ビルド

```bash
cd integrate
make
```

- ONSITE 側の `make MYHOME=$(ONSITE_DIR)` と optimizer 側の `make onsite-objs` が実行され、その後に **para** がリンクされる。
- **前提**: optimizer 側で `scripts/setup_onsite.sh` を実行済みであること（`onsite-objs` ターゲットが必要）。

### main の重複について

ONSITE と optimizer の両方に `main` がある場合はリンクで複数定義エラーになります。そのときは `integrate/Makefile.para` の **ONSITE_EXCLUDE_OBJ** に、ONSITE 側の main を含む .o を指定してください（例: `obj/main.o`）。逆に、ONSITE の main だけを使う場合は optimizer の onsite_main_onsite.o を外す必要があります（Makefile を編集）。

## ファイル

| ファイル | 説明 |
|----------|------|
| `.onsite_paths` | ONSITE_DIR / OPTIMIZER_DIR を保存（バッチが生成） |
| `Makefile.para` | 統合用 Makefile（バッチが生成。手で編集可） |
