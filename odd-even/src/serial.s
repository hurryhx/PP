	.file	"serial.cc"
	.text
	.globl	_Z4swapRiS_
	.type	_Z4swapRiS_, @function
_Z4swapRiS_:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	-24(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -4(%rbp)
	movq	-32(%rbp), %rax
	movl	(%rax), %edx
	movq	-24(%rbp), %rax
	movl	%edx, (%rax)
	movq	-32(%rbp), %rax
	movl	-4(%rbp), %edx
	movl	%edx, (%rax)
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	_Z4swapRiS_, .-_Z4swapRiS_
	.section	.rodata
.LC0:
	.string	"serial.cc"
.LC1:
	.string	"data[i-1] <= data[i]"
	.text
	.globl	main
	.type	main, @function
main:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$56, %rsp
	.cfi_offset 3, -24
	movl	$10000, -36(%rbp)
	movl	-36(%rbp), %eax
	cltq
	movabsq	$2287828610704211968, %rdx
	cmpq	%rdx, %rax
	ja	.L3
	salq	$2, %rax
	jmp	.L4
.L3:
	movq	$-1, %rax
.L4:
	movq	%rax, %rdi
	call	_Znam
	movq	%rax, -48(%rbp)
	movl	$0, -52(%rbp)
	movl	$0, -20(%rbp)
	jmp	.L5
.L6:
	movl	-20(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	movq	-48(%rbp), %rax
	leaq	(%rdx,%rax), %rbx
	call	rand
	movl	%eax, (%rbx)
	addl	$1, -20(%rbp)
.L5:
	movl	-20(%rbp), %eax
	cmpl	-36(%rbp), %eax
	jl	.L6
	movl	$0, -24(%rbp)
	jmp	.L7
.L11:
	movl	-24(%rbp), %eax
	andl	$1, %eax
	movl	%eax, %edx
	movl	$2, %eax
	subl	%edx, %eax
	movl	%eax, -56(%rbp)
	movl	-56(%rbp), %eax
	movl	%eax, -28(%rbp)
	jmp	.L8
.L10:
	movl	-28(%rbp), %eax
	cltq
	salq	$2, %rax
	leaq	-4(%rax), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movl	-28(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rcx
	movq	-48(%rbp), %rax
	addq	%rcx, %rax
	movl	(%rax), %eax
	cmpl	%eax, %edx
	jle	.L9
	movl	-28(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	movq	-48(%rbp), %rax
	addq	%rax, %rdx
	movl	-28(%rbp), %eax
	cltq
	salq	$2, %rax
	leaq	-4(%rax), %rcx
	movq	-48(%rbp), %rax
	addq	%rcx, %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	_Z4swapRiS_
.L9:
	addl	$2, -28(%rbp)
.L8:
	movl	-28(%rbp), %eax
	cmpl	-36(%rbp), %eax
	jl	.L10
	addl	$1, -24(%rbp)
.L7:
	movl	-24(%rbp), %eax
	cmpl	-36(%rbp), %eax
	jl	.L11
	movl	$0, -32(%rbp)
	jmp	.L12
.L14:
	movl	-32(%rbp), %eax
	cltq
	salq	$2, %rax
	leaq	-4(%rax), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movl	-32(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rcx
	movq	-48(%rbp), %rax
	addq	%rcx, %rax
	movl	(%rax), %eax
	cmpl	%eax, %edx
	jle	.L13
	movl	$_ZZ4mainE19__PRETTY_FUNCTION__, %ecx
	movl	$22, %edx
	movl	$.LC0, %esi
	movl	$.LC1, %edi
	call	__assert_fail
.L13:
	addl	$1, -32(%rbp)
.L12:
	movl	-32(%rbp), %eax
	cmpl	-36(%rbp), %eax
	jl	.L14
	movl	$0, %eax
	addq	$56, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	main, .-main
	.section	.rodata
	.type	_ZZ4mainE19__PRETTY_FUNCTION__, @object
	.size	_ZZ4mainE19__PRETTY_FUNCTION__, 11
_ZZ4mainE19__PRETTY_FUNCTION__:
	.string	"int main()"
	.ident	"GCC: (GNU) 4.8.0 20130502 (prerelease)"
	.section	.note.GNU-stack,"",@progbits
