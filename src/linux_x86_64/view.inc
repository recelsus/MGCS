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
    push rbx
    push r12
    push r13
    sub rsp, 8
    mov rbx, rdi
    mov r12, rsi

    lea rsi, [rip + kv_time]
    call parse_expected_key
    test rax, rax
    je .Lsplit_try_legacy
    mov rdi, rax
    mov r13, rax
    call parse_quoted_value
    test rax, rax
    je .Lsplit_try_legacy
    lea rcx, [r13 + 1]
    mov qword ptr [r12], rcx

    mov rdi, rax
    lea rsi, [rip + kv_run_id]
    call parse_expected_key
    test rax, rax
    je .Lsplit_try_legacy
    mov qword ptr [r12 + 8], rax
    mov rdi, rax
    call parse_plain_value
    test rax, rax
    je .Lsplit_try_legacy

    mov rdi, rax
    lea rsi, [rip + kv_seq]
    call parse_expected_key
    test rax, rax
    je .Lsplit_try_legacy
    mov qword ptr [r12 + 16], rax
    mov rdi, rax
    call parse_plain_value
    test rax, rax
    je .Lsplit_try_legacy

    mov rdi, rax
    lea rsi, [rip + kv_source]
    call parse_expected_key
    test rax, rax
    je .Lsplit_try_legacy
    mov rdi, rax
    mov r13, rax
    call parse_quoted_value
    test rax, rax
    je .Lsplit_try_legacy
    lea rcx, [r13 + 1]
    mov qword ptr [r12 + 24], rcx

    mov rdi, rax
    lea rsi, [rip + kv_tag]
    call parse_expected_key
    test rax, rax
    je .Lsplit_try_legacy
    mov rdi, rax
    mov r13, rax
    call parse_quoted_value
    test rax, rax
    je .Lsplit_try_legacy
    lea rcx, [r13 + 1]
    mov qword ptr [r12 + 32], rcx

    mov rdi, rax
    lea rsi, [rip + kv_cmd]
    call parse_expected_key
    test rax, rax
    je .Lsplit_try_legacy
    mov rdi, rax
    mov r13, rax
    call parse_quoted_value
    test rax, rax
    je .Lsplit_try_legacy
    lea rcx, [r13 + 1]
    mov qword ptr [r12 + 40], rcx

    mov rdi, rax
    lea rsi, [rip + kv_line]
    call parse_expected_key
    test rax, rax
    je .Lsplit_try_legacy
    mov rdi, rax
    mov r13, rax
    call parse_quoted_value
    test rax, rax
    je .Lsplit_try_legacy
    lea rcx, [r13 + 1]
    mov qword ptr [r12 + 48], rcx
    mov dl, byte ptr [rax]
    test dl, dl
    jne .Lsplit_try_legacy
    xor eax, eax
    jmp .Lsplit_done

.Lsplit_try_legacy:
    mov rdi, rbx
    mov rsi, r12
    call split_record_legacy

.Lsplit_done:
    add rsp, 8
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

split_record_legacy:
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

parse_expected_key:
    mov rax, rdi

.Lparse_key_loop:
    mov dl, byte ptr [rsi]
    test dl, dl
    je .Lparse_key_ok
    mov cl, byte ptr [rax]
    cmp cl, dl
    jne .Lparse_key_fail
    inc rax
    inc rsi
    jmp .Lparse_key_loop

.Lparse_key_ok:
    ret

.Lparse_key_fail:
    xor eax, eax
    ret

parse_plain_value:
    mov rax, rdi

.Lparse_plain_loop:
    mov dl, byte ptr [rax]
    test dl, dl
    je .Lparse_plain_fail
    cmp dl, 32
    je .Lparse_plain_cut
    cmp dl, 10
    je .Lparse_plain_fail
    inc rax
    jmp .Lparse_plain_loop

.Lparse_plain_cut:
    mov byte ptr [rax], 0
    lea rax, [rax + 1]
    ret

.Lparse_plain_fail:
    xor eax, eax
    ret

parse_quoted_value:
    mov al, byte ptr [rdi]
    cmp al, 34
    jne .Lparse_quoted_fail
    lea rax, [rdi + 1]
    mov rdx, rax

.Lparse_quoted_loop:
    mov cl, byte ptr [rax]
    test cl, cl
    je .Lparse_quoted_fail
    cmp cl, 10
    je .Lparse_quoted_fail
    cmp cl, 92
    je .Lparse_quoted_escape
    cmp cl, 34
    je .Lparse_quoted_close
    mov byte ptr [rdx], cl
    inc rdx
    inc rax
    jmp .Lparse_quoted_loop

.Lparse_quoted_escape:
    inc rax
    mov cl, byte ptr [rax]
    test cl, cl
    je .Lparse_quoted_fail
    cmp cl, 10
    je .Lparse_quoted_fail
    mov byte ptr [rdx], cl
    inc rdx
    inc rax
    jmp .Lparse_quoted_loop

.Lparse_quoted_close:
    mov byte ptr [rdx], 0
    inc rax
    mov cl, byte ptr [rax]
    test cl, cl
    je .Lparse_quoted_ok
    cmp cl, 10
    je .Lparse_quoted_newline
    cmp cl, 32
    jne .Lparse_quoted_fail
    inc rax
    jmp .Lparse_quoted_ok

.Lparse_quoted_newline:
    mov byte ptr [rax], 0

.Lparse_quoted_ok:
    ret

.Lparse_quoted_fail:
    xor eax, eax
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
    lea rsi, [rip + hdr_close]
    call write_string_fd

    mov rdi, qword ptr [rbx + 24]
    call string_is_blank_value
    test eax, eax
    jne .Lheader_skip_type
    mov edi, 1
    lea rsi, [rip + hdr_source]
    call write_string_fd
    mov edi, 1
    mov rsi, qword ptr [rbx + 24]
    call write_string_fd

.Lheader_skip_type:
    mov rdi, qword ptr [rbx + 32]
    call string_is_blank_value
    test eax, eax
    jne .Lheader_skip_tag
    mov edi, 1
    lea rsi, [rip + hdr_tag]
    call write_string_fd
    mov edi, 1
    mov rsi, qword ptr [rbx + 32]
    call write_string_fd

.Lheader_skip_tag:
    mov rdi, qword ptr [rbx + 40]
    call string_is_blank_value
    test eax, eax
    jne .Lheader_skip_cmd
    mov edi, 1
    lea rsi, [rip + hdr_cmd]
    call write_string_fd
    mov edi, 1
    mov rsi, qword ptr [rbx + 40]
    call write_string_fd

.Lheader_skip_cmd:
    mov edi, 1
    lea rsi, [rip + newline_str]
    call write_string_fd

    add rsp, 8
    pop rbx
    pop rbp
    ret
