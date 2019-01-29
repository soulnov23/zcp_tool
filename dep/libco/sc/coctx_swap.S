#define _esp 0
#define _eip 4

#define _eax 8
#define _ebx 12
#define _ecx 16
#define _edx 20

#define _edi 24
#define _esi 28

#define _ebp 32

/* di,si,dx,cx,8,9 */

#define _rsp 0
#define _rip 8

#define _rbp 16
#define _rdi 24
#define _rsi 32

#define _rdx 40
#define _rcx 48
#define _r8 56
#define _r9 64

#define _rbx 72
#define _r12 80
#define _r13 88
#define _r14 96
#define _r15 104


.globl coctx_swap 
.type  coctx_swap, @function
coctx_swap:

#if defined(__i386__)

	/* param old ctx */
	movl	4(%esp), %eax

	/* registers */
	movl	%ebx, _ebx(%eax)
	movl	%ecx, _ecx(%eax)
	movl	%edx, _edx(%eax)

	movl	%edi, _edi(%eax)
	movl	%esi, _esi(%eax)
	movl	%ebp, _ebp(%eax)

	/* sp,ip */
	movl	(%esp), %ecx
	movl	%ecx, _eip(%eax)

	leal	4(%esp), %ecx
	movl	%ecx, _esp(%eax)

	/* param new ctx */
	movl	8(%esp), %eax

	/* push ret ( ip,sp ) */
	movl	_eip(%eax), %ecx
	movl	_esp(%eax), %esp
	pushl	%ecx

	/* registers */
	movl	_ecx(%eax), %ecx
	movl	_edx(%eax), %edx

	movl	_edi(%eax), %edi
	movl	_esi(%eax), %esi
	movl	_ebp(%eax), %ebp
	movl    _ebx(%eax), %ebx
	
	movl	$0, %eax
	ret

#elif defined(__x86_64__)

	/* save registers */
	/* 1 */
	movq	%rbx, _rbx(%rdi)
	movq	%rbp, _rbp(%rdi)
	movq	%r12, _r12(%rdi)
	movq	%r13, _r13(%rdi)
	movq	%r14, _r14(%rdi)
	movq	%r15, _r15(%rdi)

	/* 2 */
	movq	%rdi, _rdi(%rdi)
	movq	%rsi, _rsi(%rdi)
	movq	%rdx, _rdx(%rdi)
	movq	%rcx, _rcx(%rdi)
	movq	%r8, _r8(%rdi)
	movq	%r9, _r9(%rdi)

	/* sp */
	movq	(%rsp), %rcx
	movq	%rcx, _rip(%rdi)
	leaq	8(%rsp), %rcx		
	movq	%rcx, _rsp(%rdi)



	/* sp */
	movq	_rip(%rsi), %rcx
	movq	_rsp(%rsi), %rsp
	pushq	%rcx

	/* registers */
	movq	_rbx(%rsi), %rbx
	movq	_rbp(%rsi), %rbp
	movq	_r12(%rsi), %r12
	movq	_r13(%rsi), %r13
	movq	_r14(%rsi), %r14
	movq	_r15(%rsi), %r15

	/* param registers */
	movq	_rdi(%rsi), %rdi
	movq	_rdx(%rsi), %rdx
	movq	_rcx(%rsi), %rcx
	movq	_r8(%rsi), %r8
	movq	_r9(%rsi), %r9

	movq	_rsi(%rsi), %rsi

	xorl	%eax, %eax
	ret

#endif

