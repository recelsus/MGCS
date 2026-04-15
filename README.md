# Maji Good Chance Summer

## Description

This application stores standard output into a target file.  
In practice, it is a small extension of something close to `echo "hello world" >> file`.

It supports both piping standard input directly into the logger and running a command while recording it with `mgcs run -- echo "hello world"`.

- Save command output
- Record `cron` execution results
- Restore and display multi-line logs
- Organize execution units with `source` / `tag` / `cmd`

Logs are stored in a one-line-per-record `key=value` format and displayed later as grouped, human-readable runs.

## Build

```bash
make build
```

## Usage

Assumes `mgcs` is available in `$PATH`.

### 1. Pipe input

```bash
ls -la | mgcs
```

With metadata:

```bash
ls -la | mgcs --source ls --tag test --cmd 'ls -la'
# --source|-s --tag|-t --cmd|-c
```

### 2. View saved logs

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

### 3. Run and save at the same time

```bash
./out/mgcs run -- ls -la
```

```bash
./out/mgcs -- ls -la
```

With metadata:

```bash
./out/mgcs -s ls -t test run -- ls -la
./out/mgcs -s ls -t test -- ls -la
```

### 4. Specify a log file

```bash
./out/mgcs --file /tmp/mgcs.log
./out/mgcs -f /tmp/mgcs.log
```

## Options

| long | short | description |
|---|---|---|
| `--source <value>` | `-s <value>` | Set the source category |
| `--tag <value>` | `-t <value>` | Set the tag |
| `--cmd <value>` | `-c <value>` | Set a command description |
| `--file <path>` | `-f <path>` | Set the log file path |
| `--view` | `-v` | View saved logs |
| `--help` | `-h` | Show help |

## Save Format

Log records are stored in a `key=value` format like this:

```text
time="2026-04-15 00:46:26" run_id=1776213986 seq=1 source="wrapped" tag="demo" cmd="/bin/sh -c printf \"wrapped one\\nwrapped two\\n\"" line="wrapped one"
time="2026-04-15 00:46:26" run_id=1776213986 seq=2 source="wrapped" tag="demo" cmd="/bin/sh -c printf \"wrapped one\\nwrapped two\\n\"" line="wrapped two"
```

Main fields:

- `time`: saved timestamp
- `run_id`: identifier for the same execution
- `seq`: line number within the run
- `source`: execution source/category
- `tag`: optional tag
- `cmd`: command description
- `line`: line body

## Case

### cron

```bash
0 * * * * /path/to/job.sh 2>&1 | /path/to/mgcs -s cron -t sync_job -c '/path/to/job.sh'
```

### manual

```bash
./out/mgcs -s manual -t test run -- ./some_script.sh --dry-run
```

### save to a specific file

```bash
./out/mgcs -f /tmp/sample.log -- echo hello
./out/mgcs -v -f /tmp/sample.log
```

## Memo

- The default log file is `$HOME/.mgcs/mgcs.log`
- `stdout` and `stderr` are recorded together
- Values containing spaces are stored with quotes

## Help

```bash
./out/mgcs --help
./out/mgcs -h
```

## License

See `LICENSE`.
