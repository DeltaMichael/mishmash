.globl print_num
.type print_num, @function

.section .text
print_num:
	movq $-8, %rcx
	# create buffer
	enter $1280, $0
	# number to print is in %rdi
	movq %rdi, %rax
dec_loop:
	# get the first decimal
	movq $0, %rdx
	movq $10, %rdi
	divq %rdi

	addq $0x30, %rdx

	# push the value on the stack
	movb %dl, (%rbp, %rcx, 1)
	# decrement the counter
	subq $8, %rcx
	# add zero to rax to check if zero
	addq $0, %rax
	jnz dec_loop
	jmp print
print:
	# syscall number is 1
	movq $1, %rax
	# load file descriptor into rdi (stdout)
	movq $1, %rdi
	# load the address of the stack pointer offset by the counter
	lea (%rbp,%rcx,1), %rsi
	# load the counter into rdx and negate to get the length
	movq %rcx, %rdx
	neg %rdx

	syscall
complete:
	movq $0, %rax
	leave
	ret
