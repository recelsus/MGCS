.section .data
opt_source:
    .quad 0
opt_tag:
    .quad 0
opt_cmd:
    .quad 0
opt_file:
    .quad 0
opt_force_view:
    .quad 0
opt_run_mode:
    .quad 0
run_argv_ptr:
    .quad 0
run_argc:
    .quad 0
resolved_file_ptr:
    .quad 0
current_run_ptr:
    .quad 0

.section .bss
    .align 8
resolved_file:
    .skip BUF_SIZE
resolved_dir:
    .skip BUF_SIZE
time_buf:
    .skip 64
run_time_raw:
    .skip 8
tm_buf:
    .skip 128
run_id_buf:
    .skip 64
seq_buf:
    .skip 32
record_fields:
    .skip 56
current_run_id:
    .skip 64
run_cmd_buf:
    .skip BUF_SIZE
pipe_fds:
    .skip 8
child_status:
    .skip 4
line_ptr:
    .skip 8
line_cap:
    .skip 8

.text
