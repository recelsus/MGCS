.equ FIELD_COUNT, 7
.equ O_CREAT_MODE, 493
.equ BUF_SIZE, 4096

.section __TEXT,__cstring,cstring_literals
str_source:
    .asciz "--source"
str_source_short:
    .asciz "-s"
str_tag:
    .asciz "--tag"
str_tag_short:
    .asciz "-t"
str_cmd:
    .asciz "--cmd"
str_cmd_short:
    .asciz "-c"
str_file:
    .asciz "--file"
str_file_short:
    .asciz "-f"
str_view:
    .asciz "--view"
str_view_short:
    .asciz "-v"
str_help:
    .asciz "--help"
str_help_short:
    .asciz "-h"
str_run:
    .asciz "run"
str_double_dash:
    .asciz "--"
mode_append:
    .asciz "a"
mode_read:
    .asciz "r"
home_name:
    .asciz "HOME"
default_dir_fmt:
    .asciz "%s/.mgcs"
default_file_fmt:
    .asciz "%s/.mgcs/mgcs.log"
blank_value:
    .asciz "-"
time_format:
    .asciz "%Y-%m-%d %H:%M:%S"
kv_time:
    .asciz "time="
kv_run_id:
    .asciz "run_id="
kv_seq:
    .asciz "seq="
kv_source:
    .asciz "source="
kv_tag:
    .asciz "tag="
kv_cmd:
    .asciz "cmd="
kv_line:
    .asciz "line="
hdr_open:
    .asciz "["
hdr_source:
    .asciz " source="
hdr_tag:
    .asciz " tag="
hdr_cmd:
    .asciz " cmd="
hdr_close:
    .asciz "]"
newline_str:
    .asciz "\n"
usage_text:
    .asciz "Maji Good Chance Summer\nUsage: mgcs [--view|-v] [--source|-s value] [--tag|-t value] [--cmd|-c value] [--file|-f path] [--help|-h]\n       mgcs [options] run -- <command> [args...]\n       mgcs [options] -- <command> [args...]"
err_missing_arg:
    .asciz "missing option argument"
err_unknown_arg:
    .asciz "unknown option"
err_missing_run_cmd:
    .asciz "run requires: run -- <command> [args...]"
err_open_log:
    .asciz "failed to open log file"
err_open_pipe:
    .asciz "failed to create pipe"
err_fork:
    .asciz "failed to fork"
err_exec:
    .asciz "failed to exec command"
err_home_missing:
    .asciz "HOME is not set"
