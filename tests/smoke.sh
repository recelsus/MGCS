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

printf 'line one\n\nline three\n' | "$BIN" --type cron --tag sync_job --cmd 'job --full' --file "$LOG_FILE"

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

printf '%s\n' "$OUTPUT" | grep 'run_id=' >/dev/null
printf '%s\n' "$OUTPUT" | grep 'type=cron' >/dev/null
printf '%s\n' "$OUTPUT" | grep 'tag=sync_job' >/dev/null
printf '%s\n' "$OUTPUT" | grep 'line one' >/dev/null
printf '%s\n' "$OUTPUT" | grep '^$' >/dev/null
printf '%s\n' "$OUTPUT" | grep 'line three' >/dev/null
