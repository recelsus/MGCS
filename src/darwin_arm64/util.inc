_trim_newline:
    stp x29, x30, [sp, -16]!
    mov x29, sp
    mov x1, x0
1:
    ldrb w2, [x1]
    cbz w2, 4f
    add x1, x1, #1
    b 1b
4:
    subs x1, x1, #1
    b.lt 9f
    ldrb w2, [x1]
    cmp w2, #10
    b.ne 5f
    strb wzr, [x1]
    subs x1, x1, #1
    b.lt 9f
5:
    ldrb w2, [x1]
    cmp w2, #13
    b.ne 9f
    strb wzr, [x1]
9:
    ldp x29, x30, [sp], 16
    ret

_value_for_type:
    adrp x8, opt_type@PAGE
    add x8, x8, opt_type@PAGEOFF
    ldr x0, [x8]
    cbnz x0, 1f
    adrp x0, blank_value@PAGE
    add x0, x0, blank_value@PAGEOFF
1:
    ret

_value_for_tag:
    adrp x8, opt_tag@PAGE
    add x8, x8, opt_tag@PAGEOFF
    ldr x0, [x8]
    cbnz x0, 1f
    adrp x0, blank_value@PAGE
    add x0, x0, blank_value@PAGEOFF
1:
    ret

_value_for_cmd:
    adrp x8, opt_cmd@PAGE
    add x8, x8, opt_cmd@PAGEOFF
    ldr x0, [x8]
    cbnz x0, 1f
    adrp x0, blank_value@PAGE
    add x0, x0, blank_value@PAGEOFF
1:
    ret

_copy_string:
    stp x29, x30, [sp, -16]!
    mov x29, sp
1:
    ldrb w2, [x0]
    strb w2, [x1]
    cbz w2, 2f
    add x0, x0, #1
    add x1, x1, #1
    b 1b
2:
    ldp x29, x30, [sp], 16
    ret

_write_string_fd:
    stp x29, x30, [sp, -32]!
    mov x29, sp
    stp x19, x20, [sp, 16]
    mov x19, x0
    mov x20, x1
    mov x0, x20
    bl _strlen
    mov x2, x0
    mov x0, x19
    mov x1, x20
    bl _write
    ldp x19, x20, [sp, 16]
    ldp x29, x30, [sp], 32
    ret

_u64_to_ascii:
    stp x29, x30, [sp, -96]!
    mov x29, sp
    stp x19, x20, [sp, 16]
    stp x21, x22, [sp, 32]

    mov x19, x0
    mov x20, x1
    add x21, sp, 48
    mov x22, #0
    mov x9, #10
    cbnz x19, 1f
    mov w10, #'0'
    strb w10, [x21]
    mov x22, #1
    b 3f
1:
    udiv x10, x19, x9
    msub x11, x10, x9, x19
    add w11, w11, #'0'
    strb w11, [x21, x22]
    add x22, x22, #1
    mov x19, x10
    cbnz x19, 1b
3:
    mov x12, #0
4:
    subs x22, x22, #1
    b.lt 5f
    ldrb w11, [x21, x22]
    strb w11, [x20, x12]
    add x12, x12, #1
    b 4b
5:
    strb wzr, [x20, x12]
    ldp x21, x22, [sp, 32]
    ldp x19, x20, [sp, 16]
    ldp x29, x30, [sp], 96
    ret
