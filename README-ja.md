# Maji Good Chance Summer

## Description

標準出力を指定のファイルに格納するアプリケーションです。
実質的には `echo "hello world" >> file` を少し拡張した程度のもの。

標準入力をそのまま記録する使い方と、`mgsc run -- echo "hello world"` でコマンドを実行しながら記録する使い方の両方に対応。

- コマンド出力の保存
- `cron` 実行結果の記録
- 複数行ログの復元表示
- `source` / `tag` / `cmd` を付けた実行単位の整理

保存は1行1レコードの`key=value`形式、参照時には実行単位ごとに整形表示。

## Build

```bash
make build
```

## Usage

$PATHに`mgcs`を配置している前提

### 1. パイプ入力

```bash
ls -la | mgcs
```

メタ情報付き:

```bash
ls -la | mgcs --source ls --tag test --cmd 'ls -la'
# --source|-s --tag|-t --cmd|-c
```

### 2. 保存済みログを表示する

```bash
mgcs
```

```bash
mgcs -v
```

```text
[2026-04-15 00:37:00] source=cron tag=sync_job cmd=job --full
line one

line three
```

### 3. 実行しながら保存する

```bash
./out/mgcs run -- ls -la
```

```bash
./out/mgcs -- ls -la
```

メタ情報付き:

```bash
./out/mgcs -s ls -t test run -- ls -la
./out/mgcs -s ls -t test -- ls -la
```

### 4. ログファイルを指定

```bash
./out/mgcs --file /tmp/mgcs.log
./out/mgcs -f /tmp/mgcs.log
```

## Options

| long| short | description |
|---|---|---|
| `--source <value>` | `-s <value>` |
| `--tag <value>` | `-t <value>` |
| `--cmd <value>` | `-c <value>` |
| `--file <path>` | `-f <path>` |
| `--view` | `-v` |
| `--help` | `-h` |

## Save Format

ログファイルには次のような `key=value` 形式で保存されます。

```text
time="2026-04-15 00:46:26" run_id=1776213986 seq=1 source="wrapped" tag="demo" cmd="/bin/sh -c printf \"wrapped one\\nwrapped two\\n\"" line="wrapped one"
time="2026-04-15 00:46:26" run_id=1776213986 seq=2 source="wrapped" tag="demo" cmd="/bin/sh -c printf \"wrapped one\\nwrapped two\\n\"" line="wrapped two"
```

主な項目:

- `time`: 保存時刻
- `run_id`: 同一実行を識別するID
- `seq`: 実行内の行番号
- `source`: 実行種別
- `tag`: 任意タグ
- `cmd`: 実行コマンド説明
- `line`: その行の本文

## Case

### cron

```bash
0 * * * * /path/to/job.sh 2>&1 | /path/to/mgcs -s cron -t sync_job -c '/path/to/job.sh'
```

### 手動

```bash
./out/mgcs -s manual -t test run -- ./some_script.sh --dry-run
```

### 特定ファイルへ分けて保存

```bash
./out/mgcs -f /tmp/sample.log -- echo hello
./out/mgcs -v -f /tmp/sample.log
```

## Memo

- 既定のログファイルは `$HOME/.mgcs/mgcs.log`
- `stdout` / `stderr` をまとめて記録します
- 値に空白を含む項目は引用符付きで保存されます

## Help

```bash
./out/mgcs --help
./out/mgcs -h
```

## License

`LICENSE` を参照
