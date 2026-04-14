_parse_args:
    stp x29, x30, [sp, -32]!
    mov x29, sp
    stp x19, x20, [sp, 16]

    mov x19, x0
    mov x20, x1
    mov x21, #1

1:
    cmp x21, x19
    bge 9f
    ldr x0, [x20, x21, lsl #3]

    adrp x1, str_type@PAGE
    add x1, x1, str_type@PAGEOFF
    bl _strcmp
    cbnz w0, 2f
    add x22, x21, #1
    cmp x22, x19
    bge 90f
    ldr x8, [x20, x22, lsl #3]
    adrp x9, opt_type@PAGE
    add x9, x9, opt_type@PAGEOFF
    str x8, [x9]
    add x21, x21, #2
    b 1b
2:
    ldr x0, [x20, x21, lsl #3]
    adrp x1, str_tag@PAGE
    add x1, x1, str_tag@PAGEOFF
    bl _strcmp
    cbnz w0, 3f
    add x22, x21, #1
    cmp x22, x19
    bge 90f
    ldr x8, [x20, x22, lsl #3]
    adrp x9, opt_tag@PAGE
    add x9, x9, opt_tag@PAGEOFF
    str x8, [x9]
    add x21, x21, #2
    b 1b
3:
    ldr x0, [x20, x21, lsl #3]
    adrp x1, str_cmd@PAGE
    add x1, x1, str_cmd@PAGEOFF
    bl _strcmp
    cbnz w0, 4f
    add x22, x21, #1
    cmp x22, x19
    bge 90f
    ldr x8, [x20, x22, lsl #3]
    adrp x9, opt_cmd@PAGE
    add x9, x9, opt_cmd@PAGEOFF
    str x8, [x9]
    add x21, x21, #2
    b 1b
4:
    ldr x0, [x20, x21, lsl #3]
    adrp x1, str_file@PAGE
    add x1, x1, str_file@PAGEOFF
    bl _strcmp
    cbnz w0, 5f
    add x22, x21, #1
    cmp x22, x19
    bge 90f
    ldr x8, [x20, x22, lsl #3]
    adrp x9, opt_file@PAGE
    add x9, x9, opt_file@PAGEOFF
    str x8, [x9]
    add x21, x21, #2
    b 1b
5:
    ldr x0, [x20, x21, lsl #3]
    adrp x1, str_view@PAGE
    add x1, x1, str_view@PAGEOFF
    bl _strcmp
    cbnz w0, 6f
    adrp x8, opt_force_view@PAGE
    add x8, x8, opt_force_view@PAGEOFF
    mov x9, #1
    str x9, [x8]
    add x21, x21, #1
    b 1b
6:
    ldr x0, [x20, x21, lsl #3]
    adrp x1, str_help@PAGE
    add x1, x1, str_help@PAGEOFF
    bl _strcmp
    cbnz w0, 91f
    adrp x0, usage_text@PAGE
    add x0, x0, usage_text@PAGEOFF
    bl _puts
    mov w0, #1
    b 99f
90:
    adrp x0, err_missing_arg@PAGE
    add x0, x0, err_missing_arg@PAGEOFF
    bl _puts
    mov w0, #1
    b 99f
91:
    adrp x0, err_unknown_arg@PAGE
    add x0, x0, err_unknown_arg@PAGEOFF
    bl _puts
    mov w0, #1
    b 99f
9:
    mov w0, #0
99:
    ldp x19, x20, [sp, 16]
    ldp x29, x30, [sp], 32
    ret

_resolve_log_path:
    stp x29, x30, [sp, -32]!
    mov x29, sp
    stp x19, x20, [sp, 16]

    adrp x19, opt_file@PAGE
    add x19, x19, opt_file@PAGEOFF
    ldr x0, [x19]
    cbz x0, 1f
    adrp x8, resolved_file_ptr@PAGE
    add x8, x8, resolved_file_ptr@PAGEOFF
    str x0, [x8]
    mov w0, #0
    b 99f
1:
    adrp x0, home_name@PAGE
    add x0, x0, home_name@PAGEOFF
    bl _getenv
    mov x20, x0
    cbnz x20, 2f
    adrp x0, err_home_missing@PAGE
    add x0, x0, err_home_missing@PAGEOFF
    bl _puts
    mov w0, #1
    b 99f
2:
    adrp x0, resolved_dir@PAGE
    add x0, x0, resolved_dir@PAGEOFF
    mov x1, #BUF_SIZE
    adrp x2, default_dir_fmt@PAGE
    add x2, x2, default_dir_fmt@PAGEOFF
    mov x3, x20
    bl _snprintf
    adrp x0, resolved_dir@PAGE
    add x0, x0, resolved_dir@PAGEOFF
    mov w1, #O_CREAT_MODE
    bl _mkdir
    adrp x0, resolved_file@PAGE
    add x0, x0, resolved_file@PAGEOFF
    mov x1, #BUF_SIZE
    adrp x2, default_file_fmt@PAGE
    add x2, x2, default_file_fmt@PAGEOFF
    mov x3, x20
    bl _snprintf
    adrp x8, resolved_file_ptr@PAGE
    add x8, x8, resolved_file_ptr@PAGEOFF
    adrp x9, resolved_file@PAGE
    add x9, x9, resolved_file@PAGEOFF
    str x9, [x8]
    mov w0, #0
99:
    ldp x19, x20, [sp, 16]
    ldp x29, x30, [sp], 32
    ret
