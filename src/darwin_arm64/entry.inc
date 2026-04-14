_main:
    stp x29, x30, [sp, -16]!
    mov x29, sp

    bl _reset_state
    bl _parse_args
    cbz w0, 1f
    mov w0, #1
    b 99f
1:
    bl _resolve_log_path
    cbz w0, 2f
    mov w0, #1
    b 99f
2:
    adrp x8, opt_force_view@PAGE
    add x8, x8, opt_force_view@PAGEOFF
    ldr x8, [x8]
    cbnz x8, 20f
    mov w0, #0
    bl _isatty
    cbz w0, 3f
20:
    bl _view_logs
    b 4f
3:
    bl _capture_logs
4:
    mov w0, #0
99:
    ldp x29, x30, [sp], 16
    ret

_reset_state:
    adrp x8, opt_type@PAGE
    add x8, x8, opt_type@PAGEOFF
    str xzr, [x8]
    adrp x8, opt_tag@PAGE
    add x8, x8, opt_tag@PAGEOFF
    str xzr, [x8]
    adrp x8, opt_cmd@PAGE
    add x8, x8, opt_cmd@PAGEOFF
    str xzr, [x8]
    adrp x8, opt_file@PAGE
    add x8, x8, opt_file@PAGEOFF
    str xzr, [x8]
    adrp x8, opt_force_view@PAGE
    add x8, x8, opt_force_view@PAGEOFF
    str xzr, [x8]
    adrp x8, resolved_file_ptr@PAGE
    add x8, x8, resolved_file_ptr@PAGEOFF
    str xzr, [x8]
    adrp x8, line_ptr@PAGE
    add x8, x8, line_ptr@PAGEOFF
    str xzr, [x8]
    adrp x8, line_cap@PAGE
    add x8, x8, line_cap@PAGEOFF
    str xzr, [x8]
    adrp x8, current_run_ptr@PAGE
    add x8, x8, current_run_ptr@PAGEOFF
    str xzr, [x8]
    ret
