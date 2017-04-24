	.file	"main.c"
	.text
.Ltext0:
	.section	.rodata
.LC0:
	.string	"\nGomshell got: "
.LC1:
	.string	"%c"
	.text
	.globl	gomshell_thread
	.type	gomshell_thread, @function
gomshell_thread:
.LFB83:
	.file 1 "main.c"
	.loc 1 75 0
	.cfi_startproc
.LVL0:
	subq	$56, %rsp
	.cfi_def_cfa_offset 64
	movq	%rdi, 8(%rsp)
	.loc 1 78 0
	movq	$0, 32(%rsp)
.LVL1:
	.loc 1 79 0
	movq	$0, 40(%rsp)
.LVL2:
	.loc 1 81 0
	movl	$0, %eax
	call	IOHook_GetPrintf
.LVL3:
	movq	%rax, 32(%rsp)
.LVL4:
.L5:
.LBB6:
.LBB7:
	.file 2 "/usr/include/x86_64-linux-gnu/bits/stdio.h"
	.loc 2 46 0
	movq	stdin(%rip), %rax
	movq	%rax, %rdi
	call	_IO_getc
.LVL5:
.LBE7:
.LBE6:
	.loc 1 84 0
	movb	%al, 31(%rsp)
.LVL6:
	.loc 1 85 0
	cmpb	$10, 31(%rsp)
	je	.L3
	.loc 1 86 0
	movq	32(%rsp), %rdx
	movl	$.LC0, %edi
	movl	$0, %eax
.LVL7:
	call	*%rdx
.LVL8:
	.loc 1 87 0
	movsbl	31(%rsp), %eax
	movq	32(%rsp), %rdx
	movl	%eax, %esi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	*%rdx
.LVL9:
	.loc 1 89 0
	jmp	.L3
.L4:
.LBB8:
.LBB9:
	.loc 2 46 0
	movq	stdin(%rip), %rax
	movq	%rax, %rdi
	call	_IO_getc
.LVL10:
.LBE9:
.LBE8:
	.loc 1 90 0
	movb	%al, 31(%rsp)
.LVL11:
	.loc 1 91 0
	movsbl	31(%rsp), %eax
.LVL12:
	movq	32(%rsp), %rdx
	movl	%eax, %esi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	*%rdx
.LVL13:
.L3:
	.loc 1 89 0
	cmpb	$10, 31(%rsp)
	jne	.L4
	.loc 1 93 0
	jmp	.L5
	.cfi_endproc
.LFE83:
	.size	gomshell_thread, .-gomshell_thread
	.section	.rodata
.LC2:
	.string	"Input handler running\n"
	.text
	.globl	terminal_input_thread
	.type	terminal_input_thread, @function
terminal_input_thread:
.LFB84:
	.loc 1 100 0
	.cfi_startproc
.LVL14:
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	subq	$16, %rsp
	.cfi_def_cfa_offset 48
	.loc 1 100 0
	movq	%fs:40, %rax
	movq	%rax, 8(%rsp)
	xorl	%eax, %eax
	.loc 1 107 0
	call	IOHook_GetGetchar
.LVL15:
	movq	%rax, %rbp
.LVL16:
	.loc 1 108 0
	movl	$0, %eax
.LVL17:
	call	IOHook_GetPrintf
.LVL18:
	movq	%rax, %r12
.LVL19:
	.loc 1 109 0
	movl	$0, %eax
.LVL20:
	call	IOHook_GetGomshellInputQueue
.LVL21:
	movq	%rax, %rbx
.LVL22:
	.loc 1 111 0
	movl	$.LC2, %edi
	movl	$0, %eax
.LVL23:
	call	*%r12
.LVL24:
.L7:
	.loc 1 113 0 discriminator 1
	movl	$0, %eax
	call	*%rbp
.LVL25:
	movb	%al, 7(%rsp)
	.loc 1 114 0 discriminator 1
	movl	$-1, %edx
	leaq	7(%rsp), %rsi
	movq	%rbx, %rdi
	call	CCThreadedQueue_Insert
.LVL26:
	jmp	.L7
	.cfi_endproc
.LFE84:
	.size	terminal_input_thread, .-terminal_input_thread
	.globl	main
	.type	main, @function
main:
.LFB85:
	.loc 1 139 0
	.cfi_startproc
.LVL27:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	subq	$32, %rsp
	.cfi_def_cfa_offset 48
	.loc 1 139 0
	movq	%fs:40, %rax
	movq	%rax, 24(%rsp)
	xorl	%eax, %eax
	.loc 1 145 0
	call	IOHook_Init
.LVL28:
	.loc 1 146 0
	movl	$-1, %ebx
	.loc 1 145 0
	testl	%eax, %eax
	jne	.L11
	movl	%eax, %ebx
	.loc 1 149 0
	movl	$0, %ecx
	movl	$gomshell_thread, %edx
	movl	$0, %esi
	leaq	8(%rsp), %rdi
	call	pthread_create
.LVL29:
	.loc 1 151 0
	movl	$0, %ecx
	movl	$terminal_input_thread, %edx
	movl	$0, %esi
	leaq	16(%rsp), %rdi
	call	pthread_create
.LVL30:
	.loc 1 156 0
	movl	$0, %esi
	movq	8(%rsp), %rdi
	call	pthread_join
.LVL31:
	.loc 1 158 0
	jmp	.L11
.L11:
	.loc 1 159 0
	movl	%ebx, %eax
	movq	24(%rsp), %rdx
	xorq	%fs:40, %rdx
	je	.L12
	call	__stack_chk_fail
.LVL32:
.L12:
	addq	$32, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE85:
	.size	main, .-main
.Letext0:
	.file 3 "/usr/lib/gcc/x86_64-linux-gnu/5/include/stddef.h"
	.file 4 "/usr/include/x86_64-linux-gnu/bits/types.h"
	.file 5 "/usr/include/libio.h"
	.file 6 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h"
	.file 7 "../../CObject/liba/Class/Class.h"
	.file 8 "../../CObject/liba/util/CIQueue.h"
	.file 9 "../../CObject/liba/util/CCThreadedQueue.h"
	.file 10 "../IOHook/IOHook.h"
	.file 11 "/usr/include/stdio.h"
	.file 12 "/usr/include/pthread.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x915
	.value	0x4
	.long	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.long	.LASF108
	.byte	0xc
	.long	.LASF109
	.long	.LASF110
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.long	.LASF9
	.byte	0x3
	.byte	0xd8
	.long	0x38
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF0
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF1
	.uleb128 0x3
	.byte	0x2
	.byte	0x7
	.long	.LASF2
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x5
	.byte	0x8
	.long	0x61
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF3
	.uleb128 0x6
	.long	0x5a
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.long	.LASF4
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF5
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF6
	.uleb128 0x3
	.byte	0x2
	.byte	0x5
	.long	.LASF7
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF8
	.uleb128 0x2
	.long	.LASF10
	.byte	0x4
	.byte	0x83
	.long	0x82
	.uleb128 0x2
	.long	.LASF11
	.byte	0x4
	.byte	0x84
	.long	0x82
	.uleb128 0x7
	.byte	0x8
	.uleb128 0x5
	.byte	0x8
	.long	0x5a
	.uleb128 0x8
	.long	.LASF41
	.byte	0xd8
	.byte	0x5
	.byte	0xf1
	.long	0x224
	.uleb128 0x9
	.long	.LASF12
	.byte	0x5
	.byte	0xf2
	.long	0x4d
	.byte	0
	.uleb128 0x9
	.long	.LASF13
	.byte	0x5
	.byte	0xf7
	.long	0xa1
	.byte	0x8
	.uleb128 0x9
	.long	.LASF14
	.byte	0x5
	.byte	0xf8
	.long	0xa1
	.byte	0x10
	.uleb128 0x9
	.long	.LASF15
	.byte	0x5
	.byte	0xf9
	.long	0xa1
	.byte	0x18
	.uleb128 0x9
	.long	.LASF16
	.byte	0x5
	.byte	0xfa
	.long	0xa1
	.byte	0x20
	.uleb128 0x9
	.long	.LASF17
	.byte	0x5
	.byte	0xfb
	.long	0xa1
	.byte	0x28
	.uleb128 0x9
	.long	.LASF18
	.byte	0x5
	.byte	0xfc
	.long	0xa1
	.byte	0x30
	.uleb128 0x9
	.long	.LASF19
	.byte	0x5
	.byte	0xfd
	.long	0xa1
	.byte	0x38
	.uleb128 0x9
	.long	.LASF20
	.byte	0x5
	.byte	0xfe
	.long	0xa1
	.byte	0x40
	.uleb128 0xa
	.long	.LASF21
	.byte	0x5
	.value	0x100
	.long	0xa1
	.byte	0x48
	.uleb128 0xa
	.long	.LASF22
	.byte	0x5
	.value	0x101
	.long	0xa1
	.byte	0x50
	.uleb128 0xa
	.long	.LASF23
	.byte	0x5
	.value	0x102
	.long	0xa1
	.byte	0x58
	.uleb128 0xa
	.long	.LASF24
	.byte	0x5
	.value	0x104
	.long	0x25c
	.byte	0x60
	.uleb128 0xa
	.long	.LASF25
	.byte	0x5
	.value	0x106
	.long	0x262
	.byte	0x68
	.uleb128 0xa
	.long	.LASF26
	.byte	0x5
	.value	0x108
	.long	0x4d
	.byte	0x70
	.uleb128 0xa
	.long	.LASF27
	.byte	0x5
	.value	0x10c
	.long	0x4d
	.byte	0x74
	.uleb128 0xa
	.long	.LASF28
	.byte	0x5
	.value	0x10e
	.long	0x89
	.byte	0x78
	.uleb128 0xa
	.long	.LASF29
	.byte	0x5
	.value	0x112
	.long	0x46
	.byte	0x80
	.uleb128 0xa
	.long	.LASF30
	.byte	0x5
	.value	0x113
	.long	0x74
	.byte	0x82
	.uleb128 0xa
	.long	.LASF31
	.byte	0x5
	.value	0x114
	.long	0x268
	.byte	0x83
	.uleb128 0xa
	.long	.LASF32
	.byte	0x5
	.value	0x118
	.long	0x278
	.byte	0x88
	.uleb128 0xa
	.long	.LASF33
	.byte	0x5
	.value	0x121
	.long	0x94
	.byte	0x90
	.uleb128 0xa
	.long	.LASF34
	.byte	0x5
	.value	0x129
	.long	0x9f
	.byte	0x98
	.uleb128 0xa
	.long	.LASF35
	.byte	0x5
	.value	0x12a
	.long	0x9f
	.byte	0xa0
	.uleb128 0xa
	.long	.LASF36
	.byte	0x5
	.value	0x12b
	.long	0x9f
	.byte	0xa8
	.uleb128 0xa
	.long	.LASF37
	.byte	0x5
	.value	0x12c
	.long	0x9f
	.byte	0xb0
	.uleb128 0xa
	.long	.LASF38
	.byte	0x5
	.value	0x12e
	.long	0x2d
	.byte	0xb8
	.uleb128 0xa
	.long	.LASF39
	.byte	0x5
	.value	0x12f
	.long	0x4d
	.byte	0xc0
	.uleb128 0xa
	.long	.LASF40
	.byte	0x5
	.value	0x131
	.long	0x27e
	.byte	0xc4
	.byte	0
	.uleb128 0xb
	.long	.LASF111
	.byte	0x5
	.byte	0x96
	.uleb128 0x8
	.long	.LASF42
	.byte	0x18
	.byte	0x5
	.byte	0x9c
	.long	0x25c
	.uleb128 0x9
	.long	.LASF43
	.byte	0x5
	.byte	0x9d
	.long	0x25c
	.byte	0
	.uleb128 0x9
	.long	.LASF44
	.byte	0x5
	.byte	0x9e
	.long	0x262
	.byte	0x8
	.uleb128 0x9
	.long	.LASF45
	.byte	0x5
	.byte	0xa2
	.long	0x4d
	.byte	0x10
	.byte	0
	.uleb128 0x5
	.byte	0x8
	.long	0x22b
	.uleb128 0x5
	.byte	0x8
	.long	0xa7
	.uleb128 0xc
	.long	0x5a
	.long	0x278
	.uleb128 0xd
	.long	0x3f
	.byte	0
	.byte	0
	.uleb128 0x5
	.byte	0x8
	.long	0x224
	.uleb128 0xc
	.long	0x5a
	.long	0x28e
	.uleb128 0xd
	.long	0x3f
	.byte	0x13
	.byte	0
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF46
	.uleb128 0x2
	.long	.LASF47
	.byte	0x6
	.byte	0x3c
	.long	0x38
	.uleb128 0x8
	.long	.LASF48
	.byte	0x10
	.byte	0x6
	.byte	0x4b
	.long	0x2c5
	.uleb128 0x9
	.long	.LASF49
	.byte	0x6
	.byte	0x4d
	.long	0x2c5
	.byte	0
	.uleb128 0x9
	.long	.LASF50
	.byte	0x6
	.byte	0x4e
	.long	0x2c5
	.byte	0x8
	.byte	0
	.uleb128 0x5
	.byte	0x8
	.long	0x2a0
	.uleb128 0x2
	.long	.LASF51
	.byte	0x6
	.byte	0x4f
	.long	0x2a0
	.uleb128 0x8
	.long	.LASF52
	.byte	0x28
	.byte	0x6
	.byte	0x5c
	.long	0x343
	.uleb128 0x9
	.long	.LASF53
	.byte	0x6
	.byte	0x5e
	.long	0x4d
	.byte	0
	.uleb128 0x9
	.long	.LASF54
	.byte	0x6
	.byte	0x5f
	.long	0x6d
	.byte	0x4
	.uleb128 0x9
	.long	.LASF55
	.byte	0x6
	.byte	0x60
	.long	0x4d
	.byte	0x8
	.uleb128 0x9
	.long	.LASF56
	.byte	0x6
	.byte	0x62
	.long	0x6d
	.byte	0xc
	.uleb128 0x9
	.long	.LASF57
	.byte	0x6
	.byte	0x66
	.long	0x4d
	.byte	0x10
	.uleb128 0x9
	.long	.LASF58
	.byte	0x6
	.byte	0x68
	.long	0x7b
	.byte	0x14
	.uleb128 0x9
	.long	.LASF59
	.byte	0x6
	.byte	0x69
	.long	0x7b
	.byte	0x16
	.uleb128 0x9
	.long	.LASF60
	.byte	0x6
	.byte	0x6a
	.long	0x2cb
	.byte	0x18
	.byte	0
	.uleb128 0xe
	.byte	0x28
	.byte	0x6
	.byte	0x5a
	.long	0x36d
	.uleb128 0xf
	.long	.LASF61
	.byte	0x6
	.byte	0x7d
	.long	0x2d6
	.uleb128 0xf
	.long	.LASF62
	.byte	0x6
	.byte	0x7e
	.long	0x36d
	.uleb128 0xf
	.long	.LASF63
	.byte	0x6
	.byte	0x7f
	.long	0x82
	.byte	0
	.uleb128 0xc
	.long	0x5a
	.long	0x37d
	.uleb128 0xd
	.long	0x3f
	.byte	0x27
	.byte	0
	.uleb128 0x2
	.long	.LASF64
	.byte	0x6
	.byte	0x80
	.long	0x343
	.uleb128 0x10
	.byte	0x30
	.byte	0x6
	.byte	0x8d
	.long	0x3f1
	.uleb128 0x9
	.long	.LASF53
	.byte	0x6
	.byte	0x8f
	.long	0x4d
	.byte	0
	.uleb128 0x9
	.long	.LASF65
	.byte	0x6
	.byte	0x90
	.long	0x6d
	.byte	0x4
	.uleb128 0x9
	.long	.LASF66
	.byte	0x6
	.byte	0x91
	.long	0x3f1
	.byte	0x8
	.uleb128 0x9
	.long	.LASF67
	.byte	0x6
	.byte	0x92
	.long	0x3f1
	.byte	0x10
	.uleb128 0x9
	.long	.LASF68
	.byte	0x6
	.byte	0x93
	.long	0x3f1
	.byte	0x18
	.uleb128 0x9
	.long	.LASF69
	.byte	0x6
	.byte	0x94
	.long	0x9f
	.byte	0x20
	.uleb128 0x9
	.long	.LASF70
	.byte	0x6
	.byte	0x95
	.long	0x6d
	.byte	0x28
	.uleb128 0x9
	.long	.LASF71
	.byte	0x6
	.byte	0x96
	.long	0x6d
	.byte	0x2c
	.byte	0
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF72
	.uleb128 0xe
	.byte	0x30
	.byte	0x6
	.byte	0x8b
	.long	0x422
	.uleb128 0xf
	.long	.LASF61
	.byte	0x6
	.byte	0x97
	.long	0x388
	.uleb128 0xf
	.long	.LASF62
	.byte	0x6
	.byte	0x98
	.long	0x422
	.uleb128 0xf
	.long	.LASF63
	.byte	0x6
	.byte	0x99
	.long	0x28e
	.byte	0
	.uleb128 0xc
	.long	0x5a
	.long	0x432
	.uleb128 0xd
	.long	0x3f
	.byte	0x2f
	.byte	0
	.uleb128 0x2
	.long	.LASF73
	.byte	0x6
	.byte	0x9a
	.long	0x3f8
	.uleb128 0x5
	.byte	0x8
	.long	0x443
	.uleb128 0x11
	.uleb128 0x2
	.long	.LASF74
	.byte	0x7
	.byte	0x4a
	.long	0x44f
	.uleb128 0x5
	.byte	0x8
	.long	0x455
	.uleb128 0x12
	.long	0x460
	.uleb128 0x13
	.long	0x9f
	.byte	0
	.uleb128 0x14
	.long	.LASF75
	.byte	0x10
	.byte	0x7
	.value	0x100
	.long	0x488
	.uleb128 0x15
	.string	"_rt"
	.byte	0x7
	.value	0x104
	.long	0x9f
	.byte	0
	.uleb128 0x15
	.string	"_vo"
	.byte	0x7
	.value	0x105
	.long	0x2d
	.byte	0x8
	.byte	0
	.uleb128 0x14
	.long	.LASF76
	.byte	0x20
	.byte	0x7
	.value	0x11b
	.long	0x4bd
	.uleb128 0x15
	.string	"_cc"
	.byte	0x7
	.value	0x11f
	.long	0x460
	.byte	0
	.uleb128 0x15
	.string	"_vt"
	.byte	0x7
	.value	0x120
	.long	0x43d
	.byte	0x10
	.uleb128 0xa
	.long	.LASF77
	.byte	0x7
	.value	0x121
	.long	0x444
	.byte	0x18
	.byte	0
	.uleb128 0x14
	.long	.LASF78
	.byte	0x10
	.byte	0x7
	.value	0x18e
	.long	0x4d8
	.uleb128 0x15
	.string	"_cc"
	.byte	0x7
	.value	0x192
	.long	0x460
	.byte	0
	.byte	0
	.uleb128 0x8
	.long	.LASF79
	.byte	0x10
	.byte	0x8
	.byte	0x46
	.long	0x4f1
	.uleb128 0x9
	.long	.LASF80
	.byte	0x8
	.byte	0x4a
	.long	0x4bd
	.byte	0
	.byte	0
	.uleb128 0x5
	.byte	0x8
	.long	0x4d8
	.uleb128 0x10
	.byte	0x90
	.byte	0x9
	.byte	0x6e
	.long	0x530
	.uleb128 0x9
	.long	.LASF81
	.byte	0x9
	.byte	0x70
	.long	0x4f1
	.byte	0
	.uleb128 0x9
	.long	.LASF82
	.byte	0x9
	.byte	0x7d
	.long	0x432
	.byte	0x8
	.uleb128 0x9
	.long	.LASF83
	.byte	0x9
	.byte	0x7e
	.long	0x432
	.byte	0x38
	.uleb128 0x9
	.long	.LASF84
	.byte	0x9
	.byte	0x7f
	.long	0x37d
	.byte	0x68
	.byte	0
	.uleb128 0x8
	.long	.LASF85
	.byte	0xb0
	.byte	0x9
	.byte	0x68
	.long	0x553
	.uleb128 0x9
	.long	.LASF86
	.byte	0x9
	.byte	0x6c
	.long	0x488
	.byte	0
	.uleb128 0x16
	.string	"_"
	.byte	0x9
	.byte	0x82
	.long	0x4f7
	.byte	0x20
	.byte	0
	.uleb128 0x2
	.long	.LASF87
	.byte	0xa
	.byte	0x1c
	.long	0x55e
	.uleb128 0x5
	.byte	0x8
	.long	0x564
	.uleb128 0x17
	.long	0x4d
	.long	0x574
	.uleb128 0x13
	.long	0x54
	.uleb128 0x18
	.byte	0
	.uleb128 0x2
	.long	.LASF88
	.byte	0xa
	.byte	0x1d
	.long	0x57f
	.uleb128 0x5
	.byte	0x8
	.long	0x585
	.uleb128 0x19
	.long	0x4d
	.long	0x590
	.uleb128 0x18
	.byte	0
	.uleb128 0x1a
	.long	.LASF112
	.byte	0x2
	.byte	0x2c
	.long	0x4d
	.byte	0x3
	.uleb128 0x1b
	.long	.LASF92
	.byte	0x1
	.byte	0x4a
	.long	0x9f
	.quad	.LFB83
	.quad	.LFE83-.LFB83
	.uleb128 0x1
	.byte	0x9c
	.long	0x6cd
	.uleb128 0x1c
	.string	"arg"
	.byte	0x1
	.byte	0x4a
	.long	0x9f
	.long	.LLST0
	.uleb128 0x1d
	.string	"msg"
	.byte	0x1
	.byte	0x4d
	.long	0x5a
	.long	.LLST1
	.uleb128 0x1e
	.long	.LASF89
	.byte	0x1
	.byte	0x4e
	.long	0x553
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x1f
	.long	.LASF90
	.byte	0x1
	.byte	0x4f
	.long	0x574
	.long	.LLST2
	.uleb128 0x20
	.long	0x590
	.quad	.LBB6
	.quad	.LBE6-.LBB6
	.byte	0x1
	.byte	0x54
	.long	0x621
	.uleb128 0x21
	.quad	.LVL5
	.long	0x8b6
	.byte	0
	.uleb128 0x20
	.long	0x590
	.quad	.LBB8
	.quad	.LBE8-.LBB8
	.byte	0x1
	.byte	0x5a
	.long	0x64a
	.uleb128 0x21
	.quad	.LVL10
	.long	0x8b6
	.byte	0
	.uleb128 0x21
	.quad	.LVL3
	.long	0x8c2
	.uleb128 0x22
	.quad	.LVL8
	.uleb128 0x3
	.byte	0x91
	.sleb128 -32
	.byte	0x6
	.long	0x676
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x9
	.byte	0x3
	.quad	.LC0
	.byte	0
	.uleb128 0x22
	.quad	.LVL9
	.uleb128 0x3
	.byte	0x91
	.sleb128 -32
	.byte	0x6
	.long	0x6a3
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x9
	.byte	0x3
	.quad	.LC1
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x54
	.uleb128 0xa
	.byte	0x91
	.sleb128 -33
	.byte	0x94
	.byte	0x1
	.byte	0x8
	.byte	0x38
	.byte	0x24
	.byte	0x8
	.byte	0x38
	.byte	0x26
	.byte	0
	.uleb128 0x24
	.quad	.LVL13
	.uleb128 0x3
	.byte	0x91
	.sleb128 -32
	.byte	0x6
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x9
	.byte	0x3
	.quad	.LC1
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x54
	.uleb128 0xa
	.byte	0x91
	.sleb128 -33
	.byte	0x94
	.byte	0x1
	.byte	0x8
	.byte	0x38
	.byte	0x24
	.byte	0x8
	.byte	0x38
	.byte	0x26
	.byte	0
	.byte	0
	.uleb128 0x25
	.long	.LASF113
	.byte	0x1
	.byte	0x63
	.long	0x9f
	.quad	.LFB84
	.quad	.LFE84-.LFB84
	.uleb128 0x1
	.byte	0x9c
	.long	0x79e
	.uleb128 0x1c
	.string	"arg"
	.byte	0x1
	.byte	0x63
	.long	0x9f
	.long	.LLST3
	.uleb128 0x26
	.string	"msg"
	.byte	0x1
	.byte	0x66
	.long	0x5a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -41
	.uleb128 0x1f
	.long	.LASF89
	.byte	0x1
	.byte	0x67
	.long	0x553
	.long	.LLST4
	.uleb128 0x1f
	.long	.LASF90
	.byte	0x1
	.byte	0x68
	.long	0x574
	.long	.LLST5
	.uleb128 0x1f
	.long	.LASF91
	.byte	0x1
	.byte	0x69
	.long	0x79e
	.long	.LLST6
	.uleb128 0x21
	.quad	.LVL15
	.long	0x8cd
	.uleb128 0x21
	.quad	.LVL18
	.long	0x8c2
	.uleb128 0x21
	.quad	.LVL21
	.long	0x8d8
	.uleb128 0x22
	.quad	.LVL24
	.uleb128 0x2
	.byte	0x7c
	.sleb128 0
	.long	0x77d
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x9
	.byte	0x3
	.quad	.LC2
	.byte	0
	.uleb128 0x27
	.quad	.LVL26
	.long	0x8e3
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x73
	.sleb128 0
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x2
	.byte	0x91
	.sleb128 -41
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x2
	.byte	0x9
	.byte	0xff
	.byte	0
	.byte	0
	.uleb128 0x5
	.byte	0x8
	.long	0x530
	.uleb128 0x1b
	.long	.LASF93
	.byte	0x1
	.byte	0x8a
	.long	0x4d
	.quad	.LFB85
	.quad	.LFE85-.LFB85
	.uleb128 0x1
	.byte	0x9c
	.long	0x89a
	.uleb128 0x28
	.long	.LASF94
	.byte	0x1
	.byte	0x8a
	.long	0x4d
	.long	.LLST7
	.uleb128 0x28
	.long	.LASF95
	.byte	0x1
	.byte	0x8a
	.long	0x89a
	.long	.LLST8
	.uleb128 0x1e
	.long	.LASF96
	.byte	0x1
	.byte	0x8d
	.long	0x295
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x1e
	.long	.LASF97
	.byte	0x1
	.byte	0x8e
	.long	0x295
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x29
	.long	.LASF114
	.byte	0x1
	.byte	0x8f
	.long	0x295
	.uleb128 0x21
	.quad	.LVL28
	.long	0x8ee
	.uleb128 0x2a
	.quad	.LVL29
	.long	0x8f9
	.long	0x846
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x1
	.byte	0x30
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x9
	.byte	0x3
	.quad	gomshell_thread
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x1
	.byte	0x30
	.byte	0
	.uleb128 0x2a
	.quad	.LVL30
	.long	0x8f9
	.long	0x875
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x1
	.byte	0x30
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x9
	.byte	0x3
	.quad	terminal_input_thread
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x1
	.byte	0x30
	.byte	0
	.uleb128 0x2a
	.quad	.LVL31
	.long	0x904
	.long	0x88c
	.uleb128 0x23
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x1
	.byte	0x30
	.byte	0
	.uleb128 0x21
	.quad	.LVL32
	.long	0x90f
	.byte	0
	.uleb128 0x5
	.byte	0x8
	.long	0xa1
	.uleb128 0x2b
	.long	.LASF98
	.byte	0xb
	.byte	0xa8
	.long	0x262
	.uleb128 0x2b
	.long	.LASF99
	.byte	0xb
	.byte	0xa9
	.long	0x262
	.uleb128 0x2c
	.long	.LASF100
	.long	.LASF100
	.byte	0x5
	.value	0x1ad
	.uleb128 0x2d
	.long	.LASF101
	.long	.LASF101
	.byte	0xa
	.byte	0x21
	.uleb128 0x2d
	.long	.LASF102
	.long	.LASF102
	.byte	0xa
	.byte	0x22
	.uleb128 0x2d
	.long	.LASF103
	.long	.LASF103
	.byte	0xa
	.byte	0x25
	.uleb128 0x2d
	.long	.LASF104
	.long	.LASF104
	.byte	0x9
	.byte	0xc3
	.uleb128 0x2d
	.long	.LASF105
	.long	.LASF105
	.byte	0xa
	.byte	0x1f
	.uleb128 0x2d
	.long	.LASF106
	.long	.LASF106
	.byte	0xc
	.byte	0xe9
	.uleb128 0x2d
	.long	.LASF107
	.long	.LASF107
	.byte	0xc
	.byte	0xfa
	.uleb128 0x2e
	.long	.LASF115
	.long	.LASF115
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x17
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x26
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x20
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1c
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x1d
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x1f
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x20
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x21
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x22
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x2113
	.uleb128 0x18
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x23
	.uleb128 0x410a
	.byte	0
	.uleb128 0x2
	.uleb128 0x18
	.uleb128 0x2111
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x24
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x2113
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x25
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x87
	.uleb128 0x19
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x26
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x27
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x28
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x29
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2a
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2b
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x2c
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.byte	0
	.byte	0
	.uleb128 0x2d
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x2e
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LLST0:
	.quad	.LVL0-.Ltext0
	.quad	.LVL3-1-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL3-1-.Ltext0
	.quad	.LFE83-.Ltext0
	.value	0x2
	.byte	0x91
	.sleb128 -56
	.quad	0
	.quad	0
.LLST1:
	.quad	.LVL6-.Ltext0
	.quad	.LVL7-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL7-.Ltext0
	.quad	.LVL11-.Ltext0
	.value	0x2
	.byte	0x91
	.sleb128 -33
	.quad	.LVL11-.Ltext0
	.quad	.LVL12-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL12-.Ltext0
	.quad	.LFE83-.Ltext0
	.value	0x2
	.byte	0x91
	.sleb128 -33
	.quad	0
	.quad	0
.LLST2:
	.quad	.LVL2-.Ltext0
	.quad	.LVL4-.Ltext0
	.value	0x2
	.byte	0x91
	.sleb128 -32
	.quad	.LVL4-.Ltext0
	.quad	.LFE83-.Ltext0
	.value	0x2
	.byte	0x91
	.sleb128 -24
	.quad	0
	.quad	0
.LLST3:
	.quad	.LVL14-.Ltext0
	.quad	.LVL15-1-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL15-1-.Ltext0
	.quad	.LFE84-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	0
	.quad	0
.LLST4:
	.quad	.LVL19-.Ltext0
	.quad	.LVL20-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL20-.Ltext0
	.quad	.LFE84-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	0
	.quad	0
.LLST5:
	.quad	.LVL16-.Ltext0
	.quad	.LVL17-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL17-.Ltext0
	.quad	.LFE84-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	0
	.quad	0
.LLST6:
	.quad	.LVL22-.Ltext0
	.quad	.LVL23-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL23-.Ltext0
	.quad	.LFE84-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LLST7:
	.quad	.LVL27-.Ltext0
	.quad	.LVL28-1-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL28-1-.Ltext0
	.quad	.LFE85-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	0
	.quad	0
.LLST8:
	.quad	.LVL27-.Ltext0
	.quad	.LVL28-1-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL28-1-.Ltext0
	.quad	.LFE85-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	0
	.quad	0
	.section	.debug_aranges,"",@progbits
	.long	0x2c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x8
	.byte	0
	.value	0
	.value	0
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.quad	0
	.quad	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF86:
	.string	"cobject"
.LASF27:
	.string	"_flags2"
.LASF28:
	.string	"_old_offset"
.LASF61:
	.string	"__data"
.LASF83:
	.string	"removeCondition"
.LASF103:
	.string	"IOHook_GetGomshellInputQueue"
.LASF110:
	.string	"/home/bbruner/Dropbox/ExAlta1/GroundStation/git/albertasat-ground-station/IOController/main"
.LASF23:
	.string	"_IO_save_end"
.LASF70:
	.string	"__nwaiters"
.LASF7:
	.string	"short int"
.LASF9:
	.string	"size_t"
.LASF1:
	.string	"sizetype"
.LASF33:
	.string	"_offset"
.LASF48:
	.string	"__pthread_internal_list"
.LASF59:
	.string	"__elision"
.LASF17:
	.string	"_IO_write_ptr"
.LASF93:
	.string	"main"
.LASF64:
	.string	"pthread_mutex_t"
.LASF19:
	.string	"_IO_buf_base"
.LASF114:
	.string	"terminal_output_thread_handle"
.LASF24:
	.string	"_markers"
.LASF14:
	.string	"_IO_read_end"
.LASF68:
	.string	"__woken_seq"
.LASF108:
	.string	"GNU C99 5.4.0 20160609 -mtune=generic -march=x86-64 -g -O1 -std=c99 -fstack-protector-strong"
.LASF54:
	.string	"__count"
.LASF49:
	.string	"__prev"
.LASF85:
	.string	"CCThreadedQueue"
.LASF92:
	.string	"gomshell_thread"
.LASF94:
	.string	"argc"
.LASF50:
	.string	"__next"
.LASF96:
	.string	"gomshell_thread_handle"
.LASF71:
	.string	"__broadcast_seq"
.LASF57:
	.string	"__kind"
.LASF46:
	.string	"long long int"
.LASF107:
	.string	"pthread_join"
.LASF112:
	.string	"getchar"
.LASF32:
	.string	"_lock"
.LASF8:
	.string	"long int"
.LASF77:
	.string	"CObject_Free"
.LASF29:
	.string	"_cur_column"
.LASF67:
	.string	"__wakeup_seq"
.LASF45:
	.string	"_pos"
.LASF58:
	.string	"__spins"
.LASF95:
	.string	"argv"
.LASF87:
	.string	"IOHook_Printf_FP"
.LASF52:
	.string	"__pthread_mutex_s"
.LASF41:
	.string	"_IO_FILE"
.LASF113:
	.string	"terminal_input_thread"
.LASF4:
	.string	"unsigned char"
.LASF62:
	.string	"__size"
.LASF65:
	.string	"__futex"
.LASF6:
	.string	"signed char"
.LASF72:
	.string	"long long unsigned int"
.LASF79:
	.string	"CIQueue"
.LASF31:
	.string	"_shortbuf"
.LASF5:
	.string	"unsigned int"
.LASF42:
	.string	"_IO_marker"
.LASF105:
	.string	"IOHook_Init"
.LASF100:
	.string	"_IO_getc"
.LASF35:
	.string	"__pad2"
.LASF16:
	.string	"_IO_write_base"
.LASF40:
	.string	"_unused2"
.LASF13:
	.string	"_IO_read_ptr"
.LASF84:
	.string	"mutex"
.LASF82:
	.string	"insertCondition"
.LASF20:
	.string	"_IO_buf_end"
.LASF3:
	.string	"char"
.LASF56:
	.string	"__nusers"
.LASF81:
	.string	"backbone"
.LASF104:
	.string	"CCThreadedQueue_Insert"
.LASF43:
	.string	"_next"
.LASF34:
	.string	"__pad1"
.LASF89:
	.string	"printf_fp"
.LASF36:
	.string	"__pad3"
.LASF37:
	.string	"__pad4"
.LASF38:
	.string	"__pad5"
.LASF53:
	.string	"__lock"
.LASF2:
	.string	"short unsigned int"
.LASF102:
	.string	"IOHook_GetGetchar"
.LASF73:
	.string	"pthread_cond_t"
.LASF109:
	.string	"main.c"
.LASF0:
	.string	"long unsigned int"
.LASF74:
	.string	"CFreeType"
.LASF69:
	.string	"__mutex"
.LASF18:
	.string	"_IO_write_end"
.LASF11:
	.string	"__off64_t"
.LASF75:
	.string	"CClass"
.LASF12:
	.string	"_flags"
.LASF10:
	.string	"__off_t"
.LASF25:
	.string	"_chain"
.LASF51:
	.string	"__pthread_list_t"
.LASF66:
	.string	"__total_seq"
.LASF39:
	.string	"_mode"
.LASF88:
	.string	"IOHook_Getchar_FP"
.LASF22:
	.string	"_IO_backup_base"
.LASF98:
	.string	"stdin"
.LASF90:
	.string	"getchar_fp"
.LASF91:
	.string	"gomshell_input"
.LASF55:
	.string	"__owner"
.LASF15:
	.string	"_IO_read_base"
.LASF76:
	.string	"CObject"
.LASF60:
	.string	"__list"
.LASF30:
	.string	"_vtable_offset"
.LASF21:
	.string	"_IO_save_base"
.LASF106:
	.string	"pthread_create"
.LASF26:
	.string	"_fileno"
.LASF115:
	.string	"__stack_chk_fail"
.LASF101:
	.string	"IOHook_GetPrintf"
.LASF44:
	.string	"_sbuf"
.LASF80:
	.string	"interface"
.LASF47:
	.string	"pthread_t"
.LASF63:
	.string	"__align"
.LASF99:
	.string	"stdout"
.LASF97:
	.string	"terminal_input_thread_handle"
.LASF111:
	.string	"_IO_lock_t"
.LASF78:
	.string	"CInterface"
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.4) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
