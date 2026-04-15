_capture_logs:
    stp x29, x30, [sp, -32]!
    mov x29, sp
    stp x19, x20, [sp, 16]

    mov x0, #0
    adrp x1, mode_read@PAGE
    add x1, x1, mode_read@PAGEOFF
    bl _fdopen
    cbnz x0, 1f
    adrp x0, err_open_log@PAGE
    add x0, x0, err_open_log@PAGEOFF
    bl _perror
    mov w0, #1
    b 99f
1:
    bl _capture_stream
99:
    ldp x19, x20, [sp, 16]
    ldp x29, x30, [sp], 32
    ret

_capture_stream:
    stp x29, x30, [sp, -112]!
    mov x29, sp
    stp x19, x20, [sp, 16]
    stp x21, x22, [sp, 32]
    stp x23, x24, [sp, 48]
    stp x25, x26, [sp, 64]
    stp x27, x28, [sp, 80]
    mov x20, x0
    mov x26, #0

    bl _prepare_run_context

    adrp x8, resolved_file_ptr@PAGE
    add x8, x8, resolved_file_ptr@PAGEOFF
    ldr x0, [x8]
    adrp x1, mode_append@PAGE
    add x1, x1, mode_append@PAGEOFF
    bl _fopen
    mov x19, x0
    cbnz x19, 1f
    adrp x0, err_open_log@PAGE
    add x0, x0, err_open_log@PAGEOFF
    bl _perror
    mov x26, #1
    b 99f
1:
    mov x21, #1

2:
    adrp x0, line_ptr@PAGE
    add x0, x0, line_ptr@PAGEOFF
    adrp x1, line_cap@PAGE
    add x1, x1, line_cap@PAGEOFF
    mov x2, x20
    bl _getline
    cmp x0, #0
    ble 8f

    adrp x8, line_ptr@PAGE
    add x8, x8, line_ptr@PAGEOFF
    ldr x22, [x8]
    mov x0, x22
    bl _trim_newline

    adrp x8, opt_run_mode@PAGE
    add x8, x8, opt_run_mode@PAGEOFF
    ldr x8, [x8]
    cbz x8, 11f
    mov x0, x22
    bl _puts
11:

    adrp x0, kv_time@PAGE
    add x0, x0, kv_time@PAGEOFF
    mov x1, x19
    bl _fputs
    adrp x0, time_buf@PAGE
    add x0, x0, time_buf@PAGEOFF
    mov x1, x19
    bl _write_quoted_value
    mov w0, #32
    mov x1, x19
    bl _fputc

    adrp x0, kv_run_id@PAGE
    add x0, x0, kv_run_id@PAGEOFF
    mov x1, x19
    bl _fputs
    adrp x0, run_id_buf@PAGE
    add x0, x0, run_id_buf@PAGEOFF
    mov x1, x19
    bl _fputs
    mov w0, #32
    mov x1, x19
    bl _fputc

    adrp x0, kv_seq@PAGE
    add x0, x0, kv_seq@PAGEOFF
    mov x1, x19
    bl _fputs
    mov x0, x21
    adrp x1, seq_buf@PAGE
    add x1, x1, seq_buf@PAGEOFF
    bl _u64_to_ascii
    adrp x0, seq_buf@PAGE
    add x0, x0, seq_buf@PAGEOFF
    mov x1, x19
    bl _fputs
    mov w0, #32
    mov x1, x19
    bl _fputc

    adrp x0, kv_source@PAGE
    add x0, x0, kv_source@PAGEOFF
    mov x1, x19
    bl _fputs
    bl _value_for_source
    mov x23, x0
    mov x0, x23
    mov x1, x19
    bl _write_quoted_value
    mov w0, #32
    mov x1, x19
    bl _fputc

    adrp x0, kv_tag@PAGE
    add x0, x0, kv_tag@PAGEOFF
    mov x1, x19
    bl _fputs
    bl _value_for_tag
    mov x24, x0
    mov x0, x24
    mov x1, x19
    bl _write_quoted_value
    mov w0, #32
    mov x1, x19
    bl _fputc

    adrp x0, kv_cmd@PAGE
    add x0, x0, kv_cmd@PAGEOFF
    mov x1, x19
    bl _fputs
    bl _value_for_cmd
    mov x25, x0
    mov x0, x25
    mov x1, x19
    bl _write_quoted_value
    mov w0, #32
    mov x1, x19
    bl _fputc

    adrp x0, kv_line@PAGE
    add x0, x0, kv_line@PAGEOFF
    mov x1, x19
    bl _fputs
    mov x0, x22
    mov x1, x19
    bl _write_quoted_value
    mov w0, #10
    mov x1, x19
    bl _fputc

    add x21, x21, #1
    b 2b
8:
    mov x0, x19
    bl _fclose
99:
    mov x0, x20
    bl _fclose
    adrp x0, line_ptr@PAGE
    add x0, x0, line_ptr@PAGEOFF
    ldr x0, [x0]
    cbz x0, 100f
    bl _free
    adrp x8, line_ptr@PAGE
    add x8, x8, line_ptr@PAGEOFF
    str xzr, [x8]
100:
    mov w0, w26
    ldp x27, x28, [sp, 80]
    ldp x25, x26, [sp, 64]
    ldp x23, x24, [sp, 48]
    ldp x21, x22, [sp, 32]
    ldp x19, x20, [sp, 16]
    ldp x29, x30, [sp], 112
    ret

_run_command:
    stp x29, x30, [sp, -80]!
    mov x29, sp
    stp x19, x20, [sp, 16]
    stp x21, x22, [sp, 32]
    stp x23, x24, [sp, 48]

    adrp x0, pipe_fds@PAGE
    add x0, x0, pipe_fds@PAGEOFF
    bl _pipe
    cmp w0, #0
    b.ge 1f
    adrp x0, err_open_pipe@PAGE
    add x0, x0, err_open_pipe@PAGEOFF
    bl _perror
    mov w0, #1
    b 99f
1:
    bl _fork
    mov x23, x0
    cmp w0, #0
    b.ge 2f
    adrp x0, err_fork@PAGE
    add x0, x0, err_fork@PAGEOFF
    bl _perror
    adrp x8, pipe_fds@PAGE
    add x8, x8, pipe_fds@PAGEOFF
    ldr w0, [x8]
    bl _close
    ldr w0, [x8, 4]
    bl _close
    mov w0, #1
    b 99f
2:
    cbz x23, 10f

    adrp x8, pipe_fds@PAGE
    add x8, x8, pipe_fds@PAGEOFF
    ldr w0, [x8, 4]
    bl _close
    ldr w0, [x8]
    adrp x1, mode_read@PAGE
    add x1, x1, mode_read@PAGEOFF
    bl _fdopen
    mov x19, x0
    cbnz x19, 3f
    adrp x0, err_open_log@PAGE
    add x0, x0, err_open_log@PAGEOFF
    bl _perror
    ldr w0, [x8]
    bl _close
    mov w22, #1
    b 4f
3:
    mov x0, x19
    bl _capture_stream
    mov w22, w0
4:
    mov w0, w23
    adrp x1, child_status@PAGE
    add x1, x1, child_status@PAGEOFF
    mov x2, #0
    bl _waitpid
    cbnz w22, 8f
    adrp x8, child_status@PAGE
    add x8, x8, child_status@PAGEOFF
    ldr w0, [x8]
    and w9, w0, #127
    cbnz w9, 7f
    lsr w0, w0, #8
    and w0, w0, #255
    b 99f
7:
    mov w0, #1
    b 99f
8:
    mov w0, w22
    b 99f

10:
    adrp x8, pipe_fds@PAGE
    add x8, x8, pipe_fds@PAGEOFF
    ldr w0, [x8]
    bl _close
    ldr w0, [x8, 4]
    mov w1, #1
    bl _dup2
    ldr w0, [x8, 4]
    mov w1, #2
    bl _dup2
    ldr w0, [x8, 4]
    bl _close
    adrp x8, run_argv_ptr@PAGE
    add x8, x8, run_argv_ptr@PAGEOFF
    ldr x1, [x8]
    ldr x0, [x1]
    bl _execvp
    adrp x0, err_exec@PAGE
    add x0, x0, err_exec@PAGEOFF
    bl _perror
    mov w0, #127
    bl _exit
99:
    ldp x23, x24, [sp, 48]
    ldp x21, x22, [sp, 32]
    ldp x19, x20, [sp, 16]
    ldp x29, x30, [sp], 80
    ret

_prepare_run_context:
    stp x29, x30, [sp, -48]!
    mov x29, sp
    str x19, [sp, 16]
    mov x0, #0
    bl _time
    mov x19, x0
    adrp x8, run_time_raw@PAGE
    add x8, x8, run_time_raw@PAGEOFF
    str x19, [x8]

    adrp x0, run_time_raw@PAGE
    add x0, x0, run_time_raw@PAGEOFF
    adrp x1, tm_buf@PAGE
    add x1, x1, tm_buf@PAGEOFF
    bl _localtime_r

    adrp x1, time_buf@PAGE
    add x1, x1, time_buf@PAGEOFF
    mov x0, x1
    mov x1, #64
    adrp x2, time_format@PAGE
    add x2, x2, time_format@PAGEOFF
    adrp x3, tm_buf@PAGE
    add x3, x3, tm_buf@PAGEOFF
    bl _strftime

    mov x0, x19
    adrp x1, run_id_buf@PAGE
    add x1, x1, run_id_buf@PAGEOFF
    bl _u64_to_ascii

    ldr x19, [sp, 16]
    ldp x29, x30, [sp], 48
    ret
