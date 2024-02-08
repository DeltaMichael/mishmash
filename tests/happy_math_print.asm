extern print_num
global	_start
section	.text
_start:
	enter 256, 0
	mov qword [rbp - 128], 2
	mov qword [rbp - 192], 4
	mov rax, 8
	neg rax
	mov qword [rbp - 256], rax
	mov qword rax,[rbp - 256]
	neg rax
	mov qword rbx,[rbp - 128]
	imul rbx, rax
	mov qword rax,[rbp - 256]
	neg rax
	mov qword rcx,[rbp - 192]
	imul rcx, rax
	mov rax, rbx
	add rax, rcx
	mov qword [rbp - 64], rax
	mov qword rdi,[rbp - 64]
	call print_num
	mov qword rdi,[rbp - 256]
	call print_num
	leave
	mov rdi, 0
	mov rax, 60
	syscall
