# コイルデータ読込と起動フロー

## 概要

- 起動時に **xcoil.txt** から「年月日・コイルNO」のリストを読み込む。
- 開発者 cfg で指定した **data_path** と **binary_struct** に従い、バイナリファイルの存在チェックを行う。
- **最初に全コイルを 1 度読んで、それぞれの製品情報を出力する**処理はオンサイトで実装する。
- その後、コイルごとのモデル計算の**前**に、そのコイルのバイナリファイルを**再ロード**してから計算する。

---

## 設定（para.cfg）

| キー | 説明 |
|------|------|
| **xcoil_file** | 年月日・コイルNO リストのファイルパス（例: `xcoil.txt`） |
| **data_path** | バイナリファイルのベースディレクトリ |
| **binary_struct** | 読み込むバイナリの構造体名（拡張子に使用） |

### バイナリファイル名規則

```
{data_path}/{コイルNO 6桁}_{年月日 yyyymmdd}.{binary_struct}
```

例: コイルNO=123, 年月日=20250107, binary_struct=CoilData  
→ `data_path/000123_20250107.CoilData`

---

## ユーティリティ

### ターミナルメッセージ（util/TerminalMessage.h）

- **TerminalMessage::info(message)** … 通常メッセージ（stdout）
- **TerminalMessage::error(message)** … エラー（stderr）
- **TerminalMessage::summary(title, lines)** … 見出し + 複数行の概要（stdout）

コイルデータを最初に読んだときに、この **summary** で件数・存在有無の概要を表示する。

### データ設定（util/DataConfig.h）

- **DataConfig::load(path)** … para.cfg を読む（xcoil_file, data_path, binary_struct）
- **DataConfig::getXcoilFilePath()**
- **DataConfig::getDataPath()**
- **DataConfig::getBinaryStructName()**

### コイルリスト（util/CoilList.h）

- **CoilEntry** … `yyyymmdd`, `coil_no`
- **loadCoilListFromFile(path, out)** … xcoil.txt をパースして `vector<CoilEntry>` に格納。戻り値は件数（失敗時 -1）。  
  フォーマット: 1 行 1 件。`yyyymmdd,coil_no` または `yyyymmdd coil_no`。

### バイナリパス（util/CoilDataPath.h）

- **CoilDataPath::buildPath(dataPath, coilNo, yyyymmdd, binaryStructName)** … 上記規則でフルパスを組み立て。コイルNO は 6 桁にゼロパディング。
- **CoilDataPath::fileExists(path)** … ファイル存在チェック。

---

## 推奨プログラム起動フロー

1. **設定読込**  
   `ParaConfig::load(configPath)` の後、`DataConfig::load(configPath)` で同じ para.cfg を読む。

2. **コイルリスト読込**  
   `DataConfig::getXcoilFilePath()` のパスで `loadCoilListFromFile(..., coils)` を呼び、`vector<CoilEntry> coils` を取得。

3. **バイナリ存在チェック**  
   各 `CoilEntry` について  
   `path = CoilDataPath::buildPath(DataConfig::getDataPath(), e.coil_no, e.yyyymmdd, DataConfig::getBinaryStructName())`  
   を組み立て、`CoilDataPath::fileExists(path)` で存在チェック。

4. **最初の概要表示**  
   - コイル件数、存在するバイナリ数・欠損数などを `TerminalMessage::summary("コイルデータ読込概要", lines)` でターミナルに表示する。
   - **1 度すべてのコイルを読んで、それぞれの製品情報を出力する**処理はオンサイトで実装する（例: 各コイルのバイナリを開いて内容を集計し、ログやファイルに書き出す）。

5. **コイルごとの計算**  
   最適化やモデル計算のループで、**コイルごとに**  
   - そのコイルのバイナリファイルを**その都度再ロード**する。  
   - ロードしたデータを使ってモデル計算を行う。

---

## オンサイト実装のポイント

- **製品情報の出力**（上記 4）: 起動後、全コイルを 1 回ずつ読んで製品情報を出力する処理は、本リポジトリではインターフェースを定義せず、オンサイトの main または専用モジュールで実装する。
- **コイルごとの再ロード**（上記 5）: `IProductDataLoader::load(ProductMeta)` の実装で、`ProductMeta` にコイルNO・年月日（またはバイナリパス）を渡し、その都度バイナリを開いて `ProductLoadedData` を返す形にするとよい。
