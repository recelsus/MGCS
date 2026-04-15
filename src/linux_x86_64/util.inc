trim_newline:
    push rbp
    mov rbp, rsp
    mov rsi, rdi

.Ltrim_find_end:
    mov al, byte ptr [rsi]
    test al, al
    je .Ltrim_step_back
    inc rsi
    jmp .Ltrim_find_end

.Ltrim_step_back:
    dec rsi
    cmp rsi, rdi
    jb .Ltrim_done
    mov al, byte ptr [rsi]
    cmp al, 10
    jne .Ltrim_check_cr
    mov byte ptr [rsi], 0
    dec rsi
    cmp rsi, rdi
    jb .Ltrim_done

.Ltrim_check_cr:
    mov al, byte ptr [rsi]
    cmp al, 13
    jne .Ltrim_done
    mov byte ptr [rsi], 0

.Ltrim_done:
    pop rbp
    ret

value_for_source:
    mov rax, qword ptr [rip + opt_source]
    test rax, rax
    jne .Ltype_ret
    lea rax, [rip + blank_value]
.Ltype_ret:
    ret

value_for_tag:
    mov rax, qword ptr [rip + opt_tag]
    test rax, rax
    jne .Ltag_ret
    lea rax, [rip + blank_value]
.Ltag_ret:
    ret

value_for_cmd:
    mov rax, qword ptr [rip + opt_cmd]
    test rax, rax
    jne .Lcmd_ret
    lea rax, [rip + blank_value]
.Lcmd_ret:
    ret

copy_string:
    push rbp
    mov rbp, rsp

.Lcopy_loop:
    mov al, byte ptr [rdi]
    mov byte ptr [rsi], al
    test al, al
    je .Lcopy_done
    inc rdi
    inc rsi
    jmp .Lcopy_loop

.Lcopy_done:
    pop rbp
    ret

write_string_fd:
    push rbp
    mov rbp, rsp
    push rbx
    sub rsp, 8
    mov ebx, edi
    mov rdx, rsi
    mov rdi, rsi
    call strlen
    mov edi, ebx
    mov rsi, rdx
    mov rdx, rax
    call write
    add rsp, 8
    pop rbx
    pop rbp
    ret

write_quoted_value:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    sub rsp, 8
    mov r12, rdi
    mov rbx, rsi

    mov edi, 34
    mov rsi, rbx
    call fputc

.Lquoted_loop:
    mov al, byte ptr [r12]
    test al, al
    je .Lquoted_done
    cmp al, 92
    je .Lquoted_escape
    cmp al, 34
    jne .Lquoted_emit

.Lquoted_escape:
    mov edi, 92
    mov rsi, rbx
    call fputc

.Lquoted_emit:
    movzx edi, byte ptr [r12]
    mov rsi, rbx
    call fputc
    inc r12
    jmp .Lquoted_loop

.Lquoted_done:
    mov edi, 34
    mov rsi, rbx
    call fputc
    add rsp, 8
    pop r12
    pop rbx
    pop rbp
    ret

string_is_blank_value:
    mov al, byte ptr [rdi]
    cmp al, '-'
    jne .Lblank_no
    mov al, byte ptr [rdi + 1]
    test al, al
    jne .Lblank_no
    mov eax, 1
    ret

.Lblank_no:
    xor eax, eax
    ret

build_run_command_string:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15

    lea r12, [rip + run_cmd_buf]
    mov r13, r12
    mov r14d, BUF_SIZE - 1
    mov r15, qword ptr [rip + run_argv_ptr]
    mov rbx, qword ptr [rip + run_argc]
    xor r8d, r8d

.Lbuild_cmd_arg_loop:
    cmp r8, rbx
    jge .Lbuild_cmd_done
    test r8, r8
    je .Lbuild_cmd_copy
    test r14d, r14d
    jle .Lbuild_cmd_done
    mov byte ptr [r13], ' '
    inc r13
    dec r14d

.Lbuild_cmd_copy:
    mov rax, qword ptr [r15 + r8 * 8]

.Lbuild_cmd_char_loop:
    test r14d, r14d
    jle .Lbuild_cmd_done
    mov dl, byte ptr [rax]
    test dl, dl
    je .Lbuild_cmd_next_arg
    mov byte ptr [r13], dl
    inc r13
    inc rax
    dec r14d
    jmp .Lbuild_cmd_char_loop

.Lbuild_cmd_next_arg:
    inc r8
    jmp .Lbuild_cmd_arg_loop

.Lbuild_cmd_done:
    mov byte ptr [r13], 0
    lea rax, [rip + run_cmd_buf]
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

u64_to_ascii:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    sub rsp, 40

    mov rbx, rdi
    mov r12, rsi
    lea r13, [rsp]
    xor r14d, r14d
    mov ecx, 10
    test rbx, rbx
    jne .Lu64_convert
    mov byte ptr [r13], '0'
    mov r14d, 1
    jmp .Lu64_write_out

.Lu64_convert:
    xor edx, edx
    mov rax, rbx
    div rcx
    add dl, '0'
    mov byte ptr [r13 + r14], dl
    inc r14
    mov rbx, rax
    test rbx, rbx
    jne .Lu64_convert

.Lu64_write_out:
    xor eax, eax

.Lu64_reverse:
    dec r14
    js .Lu64_terminate
    mov dl, byte ptr [r13 + r14]
    mov byte ptr [r12 + rax], dl
    inc rax
    jmp .Lu64_reverse

.Lu64_terminate:
    mov byte ptr [r12 + rax], 0
    add rsp, 40
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
