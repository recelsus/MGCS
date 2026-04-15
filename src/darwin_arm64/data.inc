.section __DATA,__data
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
resolved_file:
    .space BUF_SIZE
resolved_dir:
    .space BUF_SIZE
time_buf:
    .space 64
run_time_raw:
    .quad 0
tm_buf:
    .space 128
run_id_buf:
    .space 64
seq_buf:
    .space 32
record_fields:
    .space 56
current_run_id:
    .space 64
run_cmd_buf:
    .space BUF_SIZE
pipe_fds:
    .space 8
child_status:
    .long 0
line_ptr:
    .quad 0
line_cap:
    .quad 0
current_run_ptr:
    .quad 0

.section __TEXT,__text
