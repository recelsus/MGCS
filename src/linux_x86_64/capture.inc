capture_logs:
    push rbp
    mov rbp, rsp
    sub rsp, 8

    xor edi, edi
    lea rsi, [rip + mode_read]
    call fdopen
    test rax, rax
    jne .Lcapture_have_input
    lea rdi, [rip + err_open_log]
    call perror
    mov eax, 1
    jmp .Lcapture_done

.Lcapture_have_input:
    mov rdi, rax
    call capture_stream

.Lcapture_done:
    add rsp, 8
    pop rbp
    ret

capture_stream:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    sub rsp, 8
    mov r12, rdi
    mov dword ptr [rbp - 44], 0

    call prepare_run_context

    mov rdi, qword ptr [rip + resolved_file_ptr]
    lea rsi, [rip + mode_append]
    call fopen
    mov rbx, rax
    test rbx, rbx
    jne .Lcapture_open_ok
    lea rdi, [rip + err_open_log]
    call perror
    mov dword ptr [rbp - 44], 1
    jmp .Lcapture_cleanup

.Lcapture_open_ok:
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

    mov rax, qword ptr [rip + opt_run_mode]
    test rax, rax
    je .Lcapture_skip_echo
    mov rdi, r14
    call puts

.Lcapture_skip_echo:

    lea rdi, [rip + kv_time]
    mov rsi, rbx
    call fputs
    lea rdi, [rip + time_buf]
    mov rsi, rbx
    call write_quoted_value
    mov edi, 32
    mov rsi, rbx
    call fputc

    lea rdi, [rip + kv_run_id]
    mov rsi, rbx
    call fputs
    lea rdi, [rip + run_id_buf]
    mov rsi, rbx
    call fputs
    mov edi, 32
    mov rsi, rbx
    call fputc

    lea rdi, [rip + kv_seq]
    mov rsi, rbx
    call fputs
    mov rdi, r13
    lea rsi, [rip + seq_buf]
    call u64_to_ascii
    lea rdi, [rip + seq_buf]
    mov rsi, rbx
    call fputs
    mov edi, 32
    mov rsi, rbx
    call fputc

    lea rdi, [rip + kv_source]
    mov rsi, rbx
    call fputs
    call value_for_source
    mov r15, rax
    mov rdi, r15
    mov rsi, rbx
    call write_quoted_value
    mov edi, 32
    mov rsi, rbx
    call fputc

    lea rdi, [rip + kv_tag]
    mov rsi, rbx
    call fputs
    call value_for_tag
    mov r15, rax
    mov rdi, r15
    mov rsi, rbx
    call write_quoted_value
    mov edi, 32
    mov rsi, rbx
    call fputc

    lea rdi, [rip + kv_cmd]
    mov rsi, rbx
    call fputs
    call value_for_cmd
    mov r15, rax
    mov rdi, r15
    mov rsi, rbx
    call write_quoted_value
    mov edi, 32
    mov rsi, rbx
    call fputc

    lea rdi, [rip + kv_line]
    mov rsi, rbx
    call fputs
    mov rdi, r14
    mov rsi, rbx
    call write_quoted_value
    mov edi, 10
    mov rsi, rbx
    call fputc

    inc r13
    jmp .Lcapture_read_loop

.Lcapture_close_file:
    mov rdi, rbx
    call fclose

.Lcapture_cleanup:
    mov rdi, r12
    call fclose
    mov rdi, qword ptr [rip + line_ptr]
    test rdi, rdi
    je .Lcapture_ok
    call free
    mov qword ptr [rip + line_ptr], 0

.Lcapture_ok:
    mov eax, dword ptr [rbp - 44]
    add rsp, 8
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

run_command:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    sub rsp, 8

    lea rdi, [rip + pipe_fds]
    call pipe
    test eax, eax
    jns .Lrun_pipe_ok
    lea rdi, [rip + err_open_pipe]
    call perror
    mov eax, 1
    jmp .Lrun_done

.Lrun_pipe_ok:
    call fork
    mov r13d, eax
    test eax, eax
    jns .Lrun_fork_ok
    lea rdi, [rip + err_fork]
    call perror
    mov edi, dword ptr [rip + pipe_fds]
    call close
    mov edi, dword ptr [rip + pipe_fds + 4]
    call close
    mov eax, 1
    jmp .Lrun_done

.Lrun_fork_ok:
    test r13d, r13d
    je .Lrun_child

    mov edi, dword ptr [rip + pipe_fds + 4]
    call close
    mov edi, dword ptr [rip + pipe_fds]
    lea rsi, [rip + mode_read]
    call fdopen
    mov r12, rax
    test r12, r12
    jne .Lrun_parent_capture
    lea rdi, [rip + err_open_log]
    call perror
    mov edi, dword ptr [rip + pipe_fds]
    call close
    mov ebx, 1
    jmp .Lrun_wait

.Lrun_parent_capture:
    mov rdi, r12
    call capture_stream
    mov ebx, eax

.Lrun_wait:
    mov edi, r13d
    lea rsi, [rip + child_status]
    xor edx, edx
    call waitpid
    test ebx, ebx
    jne .Lrun_capture_fail
    mov eax, dword ptr [rip + child_status]
    mov ecx, eax
    and ecx, 127
    cmp ecx, 0
    jne .Lrun_signal_fail
    shr eax, 8
    and eax, 255
    jmp .Lrun_done

.Lrun_capture_fail:
    mov eax, ebx
    jmp .Lrun_done

.Lrun_signal_fail:
    mov eax, 1
    jmp .Lrun_done

.Lrun_child:
    mov edi, dword ptr [rip + pipe_fds]
    call close
    mov edi, dword ptr [rip + pipe_fds + 4]
    mov esi, 1
    call dup2
    mov edi, dword ptr [rip + pipe_fds + 4]
    mov esi, 2
    call dup2
    mov edi, dword ptr [rip + pipe_fds + 4]
    call close
    mov rsi, qword ptr [rip + run_argv_ptr]
    mov rdi, qword ptr [rsi]
    call execvp
    lea rdi, [rip + err_exec]
    call perror
    mov edi, 127
    call exit

.Lrun_done:
    add rsp, 8
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
    mov qword ptr [rip + run_time_raw], rbx

    lea rdi, [rip + run_time_raw]
    lea rsi, [rip + tm_buf]
    call localtime_r

    lea rdi, [rip + time_buf]
    mov esi, 64
    lea rdx, [rip + time_format]
    lea rcx, [rip + tm_buf]
    call strftime

    mov rdi, rbx
    lea rsi, [rip + run_id_buf]
    call u64_to_ascii

    add rsp, 8
    pop rbx
    pop rbp
    ret
