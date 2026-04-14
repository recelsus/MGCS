.equ FIELD_COUNT, 7
.equ O_CREAT_MODE, 493
.equ BUF_SIZE, 4096

.section __TEXT,__cstring,cstring_literals
str_type:
    .asciz "--type"
str_tag:
    .asciz "--tag"
str_cmd:
    .asciz "--cmd"
str_file:
    .asciz "--file"
str_view:
    .asciz "--view"
str_help:
    .asciz "--help"
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
hdr_open:
    .asciz "["
hdr_run_id:
    .asciz "] run_id="
hdr_type:
    .asciz " type="
hdr_tag:
    .asciz " tag="
hdr_cmd:
    .asciz " cmd="
newline_str:
    .asciz "\n"
usage_text:
    .asciz "Usage: mgcs [--view] [--type value] [--tag value] [--cmd value] [--file path]"
err_missing_arg:
    .asciz "missing option argument"
err_unknown_arg:
    .asciz "unknown option"
err_open_log:
    .asciz "failed to open log file"
err_home_missing:
    .asciz "HOME is not set"
