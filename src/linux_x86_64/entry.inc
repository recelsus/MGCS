main:
    push rbp
    mov rbp, rsp

    call reset_state
    call parse_args
    test eax, eax
    je .Lmain_args_ok
    mov eax, 1
    jmp .Lmain_done

.Lmain_args_ok:
    call resolve_log_path
    test eax, eax
    je .Lmain_path_ok
    mov eax, 1
    jmp .Lmain_done

.Lmain_path_ok:
    mov rax, qword ptr [rip + opt_run_mode]
    test rax, rax
    jne .Lmain_run
    mov rax, qword ptr [rip + opt_force_view]
    test rax, rax
    jne .Lmain_view
    xor edi, edi
    call isatty
    test eax, eax
    je .Lmain_capture

.Lmain_view:
    call view_logs
    xor eax, eax
    jmp .Lmain_done

.Lmain_run:
    call run_command
    jmp .Lmain_done

.Lmain_capture:
    call capture_logs
    xor eax, eax

.Lmain_done:
    pop rbp
    ret

reset_state:
    mov qword ptr [rip + opt_source], 0
    mov qword ptr [rip + opt_tag], 0
    mov qword ptr [rip + opt_cmd], 0
    mov qword ptr [rip + opt_file], 0
    mov qword ptr [rip + opt_force_view], 0
    mov qword ptr [rip + opt_run_mode], 0
    mov qword ptr [rip + run_argv_ptr], 0
    mov qword ptr [rip + run_argc], 0
    mov qword ptr [rip + resolved_file_ptr], 0
    mov qword ptr [rip + line_ptr], 0
    mov qword ptr [rip + line_cap], 0
    mov qword ptr [rip + current_run_ptr], 0
    ret
