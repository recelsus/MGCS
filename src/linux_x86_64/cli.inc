parse_args:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    sub rsp, 8

    mov ebx, edi
    mov r12, rsi
    mov r13d, 1

.Lparse_loop:
    cmp r13d, ebx
    jge .Lparse_ok

    mov rdi, qword ptr [r12 + r13 * 8]
    lea rsi, [rip + str_source]
    call strcmp
    test eax, eax
    je .Lparse_set_type
    mov rdi, qword ptr [r12 + r13 * 8]
    lea rsi, [rip + str_source_short]
    call strcmp
    test eax, eax
    jne .Lparse_check_tag

.Lparse_set_type:
    lea r14d, [r13 + 1]
    cmp r14d, ebx
    jge .Lparse_missing_arg
    mov rax, qword ptr [r12 + r14 * 8]
    mov qword ptr [rip + opt_source], rax
    add r13d, 2
    jmp .Lparse_loop

.Lparse_check_tag:
    mov rdi, qword ptr [r12 + r13 * 8]
    lea rsi, [rip + str_tag]
    call strcmp
    test eax, eax
    je .Lparse_set_tag
    mov rdi, qword ptr [r12 + r13 * 8]
    lea rsi, [rip + str_tag_short]
    call strcmp
    test eax, eax
    jne .Lparse_check_cmd

.Lparse_set_tag:
    lea r14d, [r13 + 1]
    cmp r14d, ebx
    jge .Lparse_missing_arg
    mov rax, qword ptr [r12 + r14 * 8]
    mov qword ptr [rip + opt_tag], rax
    add r13d, 2
    jmp .Lparse_loop

.Lparse_check_cmd:
    mov rdi, qword ptr [r12 + r13 * 8]
    lea rsi, [rip + str_cmd]
    call strcmp
    test eax, eax
    je .Lparse_set_cmd
    mov rdi, qword ptr [r12 + r13 * 8]
    lea rsi, [rip + str_cmd_short]
    call strcmp
    test eax, eax
    jne .Lparse_check_file

.Lparse_set_cmd:
    lea r14d, [r13 + 1]
    cmp r14d, ebx
    jge .Lparse_missing_arg
    mov rax, qword ptr [r12 + r14 * 8]
    mov qword ptr [rip + opt_cmd], rax
    add r13d, 2
    jmp .Lparse_loop

.Lparse_check_file:
    mov rdi, qword ptr [r12 + r13 * 8]
    lea rsi, [rip + str_file]
    call strcmp
    test eax, eax
    je .Lparse_set_file
    mov rdi, qword ptr [r12 + r13 * 8]
    lea rsi, [rip + str_file_short]
    call strcmp
    test eax, eax
    jne .Lparse_check_view

.Lparse_set_file:
    lea r14d, [r13 + 1]
    cmp r14d, ebx
    jge .Lparse_missing_arg
    mov rax, qword ptr [r12 + r14 * 8]
    mov qword ptr [rip + opt_file], rax
    add r13d, 2
    jmp .Lparse_loop

.Lparse_check_view:
    mov rdi, qword ptr [r12 + r13 * 8]
    lea rsi, [rip + str_view]
    call strcmp
    test eax, eax
    je .Lparse_set_view
    mov rdi, qword ptr [r12 + r13 * 8]
    lea rsi, [rip + str_view_short]
    call strcmp
    test eax, eax
    jne .Lparse_check_help

.Lparse_set_view:
    mov qword ptr [rip + opt_force_view], 1
    add r13d, 1
    jmp .Lparse_loop

.Lparse_check_help:
    mov rdi, qword ptr [r12 + r13 * 8]
    lea rsi, [rip + str_help]
    call strcmp
    test eax, eax
    je .Lparse_show_help
    mov rdi, qword ptr [r12 + r13 * 8]
    lea rsi, [rip + str_help_short]
    call strcmp
    test eax, eax
    jne .Lparse_check_run

.Lparse_show_help:
    lea rdi, [rip + usage_text]
    call puts
    mov eax, 1
    jmp .Lparse_done

.Lparse_check_run:
    mov rdi, qword ptr [r12 + r13 * 8]
    lea rsi, [rip + str_run]
    call strcmp
    test eax, eax
    jne .Lparse_check_double_dash
    lea r14d, [r13 + 1]
    cmp r14d, ebx
    jge .Lparse_missing_run
    mov rdi, qword ptr [r12 + r14 * 8]
    lea rsi, [rip + str_double_dash]
    call strcmp
    test eax, eax
    jne .Lparse_missing_run
    lea r14d, [r13 + 2]
    cmp r14d, ebx
    jge .Lparse_missing_run
    mov qword ptr [rip + opt_run_mode], 1
    lea rax, [r12 + r14 * 8]
    mov qword ptr [rip + run_argv_ptr], rax
    mov eax, ebx
    sub eax, r14d
    cdqe
    mov qword ptr [rip + run_argc], rax
    mov rax, qword ptr [rip + opt_cmd]
    test rax, rax
    jne .Lparse_ok
    call build_run_command_string
    mov qword ptr [rip + opt_cmd], rax
    jmp .Lparse_ok

.Lparse_check_double_dash:
    mov rdi, qword ptr [r12 + r13 * 8]
    lea rsi, [rip + str_double_dash]
    call strcmp
    test eax, eax
    jne .Lparse_unknown_arg
    lea r14d, [r13 + 1]
    cmp r14d, ebx
    jge .Lparse_missing_run
    mov qword ptr [rip + opt_run_mode], 1
    lea rax, [r12 + r14 * 8]
    mov qword ptr [rip + run_argv_ptr], rax
    mov eax, ebx
    sub eax, r14d
    cdqe
    mov qword ptr [rip + run_argc], rax
    mov rax, qword ptr [rip + opt_cmd]
    test rax, rax
    jne .Lparse_ok
    call build_run_command_string
    mov qword ptr [rip + opt_cmd], rax
    jmp .Lparse_ok

.Lparse_missing_run:
    lea rdi, [rip + err_missing_run_cmd]
    call puts
    mov eax, 1
    jmp .Lparse_done

.Lparse_missing_arg:
    lea rdi, [rip + err_missing_arg]
    call puts
    mov eax, 1
    jmp .Lparse_done

.Lparse_unknown_arg:
    lea rdi, [rip + err_unknown_arg]
    call puts
    mov eax, 1
    jmp .Lparse_done

.Lparse_ok:
    xor eax, eax

.Lparse_done:
    add rsp, 8
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

resolve_log_path:
    push rbp
    mov rbp, rsp
    push rbx
    sub rsp, 8

    mov rax, qword ptr [rip + opt_file]
    test rax, rax
    je .Lresolve_use_home
    mov qword ptr [rip + resolved_file_ptr], rax
    xor eax, eax
    jmp .Lresolve_done

.Lresolve_use_home:
    lea rdi, [rip + home_name]
    call getenv
    mov rbx, rax
    test rbx, rbx
    jne .Lresolve_have_home
    lea rdi, [rip + err_home_missing]
    call puts
    mov eax, 1
    jmp .Lresolve_done

.Lresolve_have_home:
    lea rdi, [rip + resolved_dir]
    mov esi, BUF_SIZE
    lea rdx, [rip + default_dir_fmt]
    mov rcx, rbx
    xor eax, eax
    call snprintf

    lea rdi, [rip + resolved_dir]
    mov esi, O_CREAT_MODE
    call mkdir

    lea rdi, [rip + resolved_file]
    mov esi, BUF_SIZE
    lea rdx, [rip + default_file_fmt]
    mov rcx, rbx
    xor eax, eax
    call snprintf

    lea rax, [rip + resolved_file]
    mov qword ptr [rip + resolved_file_ptr], rax
    xor eax, eax

.Lresolve_done:
    add rsp, 8
    pop rbx
    pop rbp
    ret
