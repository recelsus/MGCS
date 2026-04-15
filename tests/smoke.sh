#!/bin/sh

set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BIN="$ROOT_DIR/out/mgcs"
TMP_DIR=$(mktemp -d /tmp/mgcs.XXXXXX)
LOG_FILE="$TMP_DIR/mgcs.log"

cleanup() {
    rm -rf "$TMP_DIR"
}

trap cleanup EXIT INT TERM

printf 'line one\n\nline three\n' | "$BIN" --source cron --tag sync_job --cmd 'job --full' --file "$LOG_FILE"

grep '^time="[0-9][0-9][0-9][0-9]-' "$LOG_FILE" >/dev/null
grep 'run_id=' "$LOG_FILE" >/dev/null
grep 'seq=1' "$LOG_FILE" >/dev/null
grep 'source="cron"' "$LOG_FILE" >/dev/null
grep 'tag="sync_job"' "$LOG_FILE" >/dev/null
grep 'cmd="job --full"' "$LOG_FILE" >/dev/null
grep 'line="line one"' "$LOG_FILE" >/dev/null
grep 'line=""' "$LOG_FILE" >/dev/null
grep 'line="line three"' "$LOG_FILE" >/dev/null

OUTPUT=$(python3 - "$BIN" "$LOG_FILE" <<'PY'
import os
import pty
import subprocess
import sys

bin_path = sys.argv[1]
log_file = sys.argv[2]
master_fd, slave_fd = pty.openpty()
proc = subprocess.Popen(
    [bin_path, "--view", "--file", log_file],
    stdin=slave_fd,
    stdout=slave_fd,
    stderr=slave_fd,
    close_fds=True,
)
os.close(slave_fd)
chunks = []
while True:
    try:
        data = os.read(master_fd, 4096)
    except OSError:
        break
    if not data:
        break
    chunks.append(data)
os.close(master_fd)
proc.wait()
sys.stdout.write(b"".join(chunks).decode("utf-8", "replace").replace("\r", ""))
PY
)

printf '%s\n' "$OUTPUT" | grep '^\[[0-9][0-9][0-9][0-9]-' >/dev/null
printf '%s\n' "$OUTPUT" | grep 'source=cron' >/dev/null
printf '%s\n' "$OUTPUT" | grep 'tag=sync_job' >/dev/null
printf '%s\n' "$OUTPUT" | grep 'cmd=job --full' >/dev/null
printf '%s\n' "$OUTPUT" | grep 'line one' >/dev/null
printf '%s\n' "$OUTPUT" | grep '^$' >/dev/null
printf '%s\n' "$OUTPUT" | grep 'line three' >/dev/null

SHORT_LOG_FILE="$TMP_DIR/mgcs-short.log"
printf 'short form\n' | "$BIN" -s short -t alias -c 'echo short' -f "$SHORT_LOG_FILE"
grep 'source="short"' "$SHORT_LOG_FILE" >/dev/null
grep 'tag="alias"' "$SHORT_LOG_FILE" >/dev/null
grep 'cmd="echo short"' "$SHORT_LOG_FILE" >/dev/null
grep 'line="short form"' "$SHORT_LOG_FILE" >/dev/null

"$BIN" -h | grep -- '--source|-s' >/dev/null
"$BIN" -h | grep -- '--tag|-t' >/dev/null
OUTPUT_SHORT=$(python3 - "$BIN" "$SHORT_LOG_FILE" <<'PY'
import os
import pty
import subprocess
import sys

bin_path = sys.argv[1]
log_file = sys.argv[2]
master_fd, slave_fd = pty.openpty()
proc = subprocess.Popen(
    [bin_path, "-v", "-f", log_file],
    stdin=slave_fd,
    stdout=slave_fd,
    stderr=slave_fd,
    close_fds=True,
)
os.close(slave_fd)
chunks = []
while True:
    try:
        data = os.read(master_fd, 4096)
    except OSError:
        break
    if not data:
        break
    chunks.append(data)
os.close(master_fd)
proc.wait()
sys.stdout.write(b"".join(chunks).decode("utf-8", "replace").replace("\r", ""))
PY
)
printf '%s\n' "$OUTPUT_SHORT" | grep 'source=short' >/dev/null
printf '%s\n' "$OUTPUT_SHORT" | grep 'tag=alias' >/dev/null

RUN_LOG_FILE="$TMP_DIR/mgcs-run.log"
RUN_OUTPUT=$("$BIN" --source wrapped --tag demo --file "$RUN_LOG_FILE" run -- /bin/sh -c 'printf "wrapped one\nwrapped two\n"')

grep 'source="wrapped"' "$RUN_LOG_FILE" >/dev/null
grep 'tag="demo"' "$RUN_LOG_FILE" >/dev/null
grep -F 'cmd="/bin/sh -c printf \"wrapped one\\nwrapped two\\n\""' "$RUN_LOG_FILE" >/dev/null
grep 'line="wrapped one"' "$RUN_LOG_FILE" >/dev/null
grep 'line="wrapped two"' "$RUN_LOG_FILE" >/dev/null
printf '%s\n' "$RUN_OUTPUT" | grep '^wrapped one$' >/dev/null
printf '%s\n' "$RUN_OUTPUT" | grep '^wrapped two$' >/dev/null

RUN_SHORTCUT_LOG_FILE="$TMP_DIR/mgcs-run-shortcut.log"
RUN_SHORTCUT_OUTPUT=$("$BIN" -s quick -f "$RUN_SHORTCUT_LOG_FILE" -- /bin/echo shortcut)
grep 'source="quick"' "$RUN_SHORTCUT_LOG_FILE" >/dev/null
grep 'cmd="/bin/echo shortcut"' "$RUN_SHORTCUT_LOG_FILE" >/dev/null
grep 'line="shortcut"' "$RUN_SHORTCUT_LOG_FILE" >/dev/null
printf '%s\n' "$RUN_SHORTCUT_OUTPUT" | grep '^shortcut$' >/dev/null
