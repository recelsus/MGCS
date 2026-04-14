view_logs:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    sub rsp, 8

    mov rdi, qword ptr [rip + resolved_file_ptr]
    lea rsi, [rip + mode_read]
    call fopen
    mov rbx, rax
    test rbx, rbx
    jne .Lview_open_ok
    jmp .Lview_done

.Lview_open_ok:
    xor r12d, r12d

.Lview_loop:
    lea rdi, [rip + line_ptr]
    lea rsi, [rip + line_cap]
    mov rdx, rbx
    call getline
    test rax, rax
    jle .Lview_close_file

    mov r13, qword ptr [rip + line_ptr]
    mov rdi, r13
    lea rsi, [rip + record_fields]
    call split_record
    test eax, eax
    jne .Lview_loop

    lea r14, [rip + record_fields]
    mov rdi, r14
    call record_visible
    test eax, eax
    je .Lview_loop

    mov r15, qword ptr [r14 + 8]
    mov rax, qword ptr [rip + current_run_ptr]
    test rax, rax
    jne .Lview_have_run
    mov rdi, r15
    lea rsi, [rip + current_run_id]
    call copy_string
    mov qword ptr [rip + current_run_ptr], 1
    xor r12d, r12d

.Lview_have_run:
    lea rdi, [rip + current_run_id]
    mov rsi, r15
    call strcmp
    test eax, eax
    je .Lview_same_run
    mov rdi, r15
    lea rsi, [rip + current_run_id]
    call copy_string
    xor r12d, r12d

.Lview_same_run:
    test r12d, r12d
    jne .Lview_print_line
    mov rdi, r14
    call print_header
    mov r12d, 1

.Lview_print_line:
    mov rdi, qword ptr [r14 + 48]
    call puts
    jmp .Lview_loop

.Lview_close_file:
    mov rdi, rbx
    call fclose
    mov rdi, qword ptr [rip + line_ptr]
    test rdi, rdi
    je .Lview_done
    call free

.Lview_done:
    add rsp, 8
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

split_record:
    push rbp
    mov rbp, rsp
    mov rdx, rdi
    xor rcx, rcx
    mov qword ptr [rsi], rdx

.Lsplit_field_loop:
    cmp rcx, 6
    jge .Lsplit_tail_scan

.Lsplit_scan:
    mov al, byte ptr [rdx]
    test al, al
    je .Lsplit_bad_record
    cmp al, 31
    je .Lsplit_cut_field
    cmp al, 10
    je .Lsplit_bad_record
    inc rdx
    jmp .Lsplit_scan

.Lsplit_cut_field:
    mov byte ptr [rdx], 0
    inc rdx
    inc rcx
    mov qword ptr [rsi + rcx * 8], rdx
    jmp .Lsplit_field_loop

.Lsplit_tail_scan:
    mov al, byte ptr [rdx]
    test al, al
    je .Lsplit_ok
    cmp al, 10
    je .Lsplit_tail_done
    inc rdx
    jmp .Lsplit_tail_scan

.Lsplit_tail_done:
    mov byte ptr [rdx], 0

.Lsplit_ok:
    xor eax, eax
    pop rbp
    ret

.Lsplit_bad_record:
    mov eax, 1
    pop rbp
    ret

record_visible:
    mov eax, 1
    ret

print_header:
    push rbp
    mov rbp, rsp
    push rbx
    sub rsp, 8
    mov rbx, rdi

    mov edi, 1
    lea rsi, [rip + hdr_open]
    call write_string_fd
    mov edi, 1
    mov rsi, qword ptr [rbx]
    call write_string_fd
    mov edi, 1
    lea rsi, [rip + hdr_run_id]
    call write_string_fd
    mov edi, 1
    mov rsi, qword ptr [rbx + 8]
    call write_string_fd
    mov edi, 1
    lea rsi, [rip + hdr_type]
    call write_string_fd
    mov edi, 1
    mov rsi, qword ptr [rbx + 24]
    call write_string_fd
    mov edi, 1
    lea rsi, [rip + hdr_tag]
    call write_string_fd
    mov edi, 1
    mov rsi, qword ptr [rbx + 32]
    call write_string_fd
    mov edi, 1
    lea rsi, [rip + hdr_cmd]
    call write_string_fd
    mov edi, 1
    mov rsi, qword ptr [rbx + 40]
    call write_string_fd
    mov edi, 1
    lea rsi, [rip + newline_str]
    call write_string_fd

    add rsp, 8
    pop rbx
    pop rbp
    ret
