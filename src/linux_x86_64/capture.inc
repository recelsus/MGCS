capture_logs:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    sub rsp, 8

    call prepare_run_context

    mov rdi, qword ptr [rip + resolved_file_ptr]
    lea rsi, [rip + mode_append]
    call fopen
    mov rbx, rax
    test rbx, rbx
    jne .Lcapture_open_ok
    lea rdi, [rip + err_open_log]
    call perror
    jmp .Lcapture_cleanup

.Lcapture_open_ok:
    xor edi, edi
    lea rsi, [rip + mode_read]
    call fdopen
    mov r12, rax
    mov r13, 1

.Lcapture_read_loop:
    lea rdi, [rip + line_ptr]
    lea rsi, [rip + line_cap]
    mov rdx, r12
    call getline
    test rax, rax
    jle .Lcapture_close_file

    mov r14, qword ptr [rip + line_ptr]
    mov rdi, r14
    call trim_newline

    lea rdi, [rip + time_buf]
    mov rsi, rbx
    call fputs
    mov edi, 31
    mov rsi, rbx
    call fputc

    lea rdi, [rip + run_id_buf]
    mov rsi, rbx
    call fputs
    mov edi, 31
    mov rsi, rbx
    call fputc

    mov rdi, r13
    lea rsi, [rip + seq_buf]
    call u64_to_ascii
    lea rdi, [rip + seq_buf]
    mov rsi, rbx
    call fputs
    mov edi, 31
    mov rsi, rbx
    call fputc

    call value_for_type
    mov r15, rax
    mov rdi, r15
    mov rsi, rbx
    call fputs
    mov edi, 31
    mov rsi, rbx
    call fputc

    call value_for_tag
    mov r15, rax
    mov rdi, r15
    mov rsi, rbx
    call fputs
    mov edi, 31
    mov rsi, rbx
    call fputc

    call value_for_cmd
    mov r15, rax
    mov rdi, r15
    mov rsi, rbx
    call fputs
    mov edi, 31
    mov rsi, rbx
    call fputc

    mov rdi, r14
    mov rsi, rbx
    call fputs
    mov edi, 10
    mov rsi, rbx
    call fputc

    inc r13
    jmp .Lcapture_read_loop

.Lcapture_close_file:
    mov rdi, rbx
    call fclose

.Lcapture_cleanup:
    mov rdi, qword ptr [rip + line_ptr]
    test rdi, rdi
    je .Lcapture_done
    call free

.Lcapture_done:
    add rsp, 8
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

prepare_run_context:
    push rbp
    mov rbp, rsp
    push rbx
    sub rsp, 8

    xor edi, edi
    call time
    mov rbx, rax

    mov rdi, rbx
    lea rsi, [rip + time_buf]
    call u64_to_ascii

    mov rdi, rbx
    lea rsi, [rip + run_id_buf]
    call u64_to_ascii

    add rsp, 8
    pop rbx
    pop rbp
    ret
