_view_logs:
    stp x29, x30, [sp, -96]!
    mov x29, sp
    stp x19, x20, [sp, 16]
    stp x21, x22, [sp, 32]
    stp x23, x24, [sp, 48]
    stp x25, x26, [sp, 64]

    adrp x8, resolved_file_ptr@PAGE
    add x8, x8, resolved_file_ptr@PAGEOFF
    ldr x0, [x8]
    adrp x1, mode_read@PAGE
    add x1, x1, mode_read@PAGEOFF
    bl _fopen
    mov x19, x0
    cbnz x19, 1f
    b 10f
1:
    mov x20, #0

2:
    adrp x0, line_ptr@PAGE
    add x0, x0, line_ptr@PAGEOFF
    adrp x1, line_cap@PAGE
    add x1, x1, line_cap@PAGEOFF
    mov x2, x19
    bl _getline
    cmp x0, #0
    ble 9f

    adrp x8, line_ptr@PAGE
    add x8, x8, line_ptr@PAGEOFF
    ldr x21, [x8]
    mov x0, x21
    adrp x1, record_fields@PAGE
    add x1, x1, record_fields@PAGEOFF
    bl _split_record
    cbnz w0, 2b

    adrp x22, record_fields@PAGE
    add x22, x22, record_fields@PAGEOFF
    mov x0, x22
    bl _record_visible
    cbz w0, 2b

    ldr x23, [x22, 8]
    adrp x8, current_run_ptr@PAGE
    add x8, x8, current_run_ptr@PAGEOFF
    ldr x24, [x8]
    cbnz x24, 3f
    mov x0, x23
    adrp x1, current_run_id@PAGE
    add x1, x1, current_run_id@PAGEOFF
    bl _copy_string
    mov x24, #1
    str x24, [x8]
    mov x20, #0
3:
    adrp x0, current_run_id@PAGE
    add x0, x0, current_run_id@PAGEOFF
    mov x1, x23
    bl _strcmp
    cbz w0, 4f
    mov x0, x23
    adrp x1, current_run_id@PAGE
    add x1, x1, current_run_id@PAGEOFF
    bl _copy_string
    mov x20, #0
4:
    cbnz x20, 5f
    mov x0, x22
    bl _print_header
    mov x20, #1
5:
    ldr x0, [x22, 48]
    bl _puts
    b 2b
9:
    mov x0, x19
    bl _fclose
    adrp x0, line_ptr@PAGE
    add x0, x0, line_ptr@PAGEOFF
    ldr x0, [x0]
    cbz x0, 10f
    bl _free
10:
    ldp x25, x26, [sp, 64]
    ldp x23, x24, [sp, 48]
    ldp x21, x22, [sp, 32]
    ldp x19, x20, [sp, 16]
    ldp x29, x30, [sp], 96
    ret

_split_record:
    stp x29, x30, [sp, -16]!
    mov x29, sp
    mov x2, x0
    mov x3, #0
    str x2, [x1]

1:
    cmp x3, #6
    bge 4f
2:
    ldrb w4, [x2]
    cbz w4, 8f
    cmp w4, #31
    beq 3f
    cmp w4, #10
    beq 8f
    add x2, x2, #1
    b 2b
3:
    strb wzr, [x2]
    add x2, x2, #1
    add x3, x3, #1
    str x2, [x1, x3, lsl #3]
    b 1b

4:
    ldrb w4, [x2]
    cbz w4, 7f
    cmp w4, #10
    beq 5f
    add x2, x2, #1
    b 4b
5:
    strb wzr, [x2]
7:
    mov w0, #0
    b 9f
8:
    mov w0, #1
9:
    ldp x29, x30, [sp], 16
    ret

_record_visible:
    mov w0, #1
    ret

_print_header:
    stp x29, x30, [sp, -16]!
    mov x29, sp
    mov x9, x0
    mov w0, #1
    adrp x1, hdr_open@PAGE
    add x1, x1, hdr_open@PAGEOFF
    bl _write_string_fd
    mov w0, #1
    ldr x1, [x9]
    bl _write_string_fd
    mov w0, #1
    adrp x1, hdr_run_id@PAGE
    add x1, x1, hdr_run_id@PAGEOFF
    bl _write_string_fd
    mov w0, #1
    ldr x1, [x9, 8]
    bl _write_string_fd
    mov w0, #1
    adrp x1, hdr_type@PAGE
    add x1, x1, hdr_type@PAGEOFF
    bl _write_string_fd
    mov w0, #1
    ldr x1, [x9, 24]
    bl _write_string_fd
    mov w0, #1
    adrp x1, hdr_tag@PAGE
    add x1, x1, hdr_tag@PAGEOFF
    bl _write_string_fd
    mov w0, #1
    ldr x1, [x9, 32]
    bl _write_string_fd
    mov w0, #1
    adrp x1, hdr_cmd@PAGE
    add x1, x1, hdr_cmd@PAGEOFF
    bl _write_string_fd
    mov w0, #1
    ldr x1, [x9, 40]
    bl _write_string_fd
    mov w0, #1
    adrp x1, newline_str@PAGE
    add x1, x1, newline_str@PAGEOFF
    bl _write_string_fd
    ldp x29, x30, [sp], 16
    ret
