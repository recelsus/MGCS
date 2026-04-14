_capture_logs:
    stp x29, x30, [sp, -112]!
    mov x29, sp
    stp x19, x20, [sp, 16]
    stp x21, x22, [sp, 32]
    stp x23, x24, [sp, 48]
    stp x25, x26, [sp, 64]
    stp x27, x28, [sp, 80]

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
    b 99f
1:
    mov x0, #0
    adrp x1, mode_read@PAGE
    add x1, x1, mode_read@PAGEOFF
    bl _fdopen
    mov x20, x0
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

    adrp x0, time_buf@PAGE
    add x0, x0, time_buf@PAGEOFF
    mov x1, x19
    bl _fputs
    mov w0, #31
    mov x1, x19
    bl _fputc

    adrp x0, run_id_buf@PAGE
    add x0, x0, run_id_buf@PAGEOFF
    mov x1, x19
    bl _fputs
    mov w0, #31
    mov x1, x19
    bl _fputc

    mov x0, x21
    adrp x1, seq_buf@PAGE
    add x1, x1, seq_buf@PAGEOFF
    bl _u64_to_ascii
    adrp x0, seq_buf@PAGE
    add x0, x0, seq_buf@PAGEOFF
    mov x1, x19
    bl _fputs
    mov w0, #31
    mov x1, x19
    bl _fputc

    bl _value_for_type
    mov x23, x0
    mov x0, x23
    mov x1, x19
    bl _fputs
    mov w0, #31
    mov x1, x19
    bl _fputc

    bl _value_for_tag
    mov x24, x0
    mov x0, x24
    mov x1, x19
    bl _fputs
    mov w0, #31
    mov x1, x19
    bl _fputc

    bl _value_for_cmd
    mov x25, x0
    mov x0, x25
    mov x1, x19
    bl _fputs
    mov w0, #31
    mov x1, x19
    bl _fputc

    mov x0, x22
    mov x1, x19
    bl _fputs
    mov w0, #10
    mov x1, x19
    bl _fputc

    add x21, x21, #1
    b 2b
8:
    mov x0, x19
    bl _fclose
99:
    adrp x0, line_ptr@PAGE
    add x0, x0, line_ptr@PAGEOFF
    ldr x0, [x0]
    cbz x0, 100f
    bl _free
100:
    ldp x27, x28, [sp, 80]
    ldp x25, x26, [sp, 64]
    ldp x23, x24, [sp, 48]
    ldp x21, x22, [sp, 32]
    ldp x19, x20, [sp, 16]
    ldp x29, x30, [sp], 112
    ret

_prepare_run_context:
    stp x29, x30, [sp, -48]!
    mov x29, sp
    str x19, [sp, 16]
    mov x0, #0
    bl _time
    mov x19, x0
    mov x0, x19
    adrp x1, time_buf@PAGE
    add x1, x1, time_buf@PAGEOFF
    bl _u64_to_ascii

    mov x0, x19
    adrp x1, run_id_buf@PAGE
    add x1, x1, run_id_buf@PAGEOFF
    bl _u64_to_ascii

    ldr x19, [sp, 16]
    ldp x29, x30, [sp], 48
    ret
