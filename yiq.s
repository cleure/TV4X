	.section	__TEXT,__text,regular,pure_instructions
	.section	__DWARF,__debug_frame,regular,debug
Lsection_debug_frame:
	.section	__DWARF,__debug_info,regular,debug
Lsection_info:
	.section	__DWARF,__debug_abbrev,regular,debug
Lsection_abbrev:
	.section	__DWARF,__debug_aranges,regular,debug
Lsection_aranges:
	.section	__DWARF,__debug_macinfo,regular,debug
Lsection_macinfo:
Lsection_line:
	.section	__DWARF,__debug_loc,regular,debug
Lsection_loc:
	.section	__DWARF,__debug_pubnames,regular,debug
Lsection_pubnames:
	.section	__DWARF,__debug_pubtypes,regular,debug
Lsection_pubtypes:
	.section	__DWARF,__debug_str,regular,debug
Lsection_str:
	.section	__DWARF,__debug_ranges,regular,debug
Lsection_ranges:
	.section	__TEXT,__text,regular,pure_instructions
Ltext_begin:
	.section	__DATA,__data
Ldata_begin:
	.section	__TEXT,__literal4,4byte_literals
	.align	2
LCPI1_0:
	.long	1058424226
LCPI1_1:
	.long	1050220167
LCPI1_2:
	.long	1038710997
LCPI1_3:
	.long	3196864954
LCPI1_4:
	.long	1058575221
LCPI1_5:
	.long	3198475567
LCPI1_6:
	.long	3204834132
LCPI1_7:
	.long	1046025863
LCPI1_8:
	.long	1050622820
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_tv4x_rgb_to_yiq
	.align	4, 0x90
_tv4x_rgb_to_yiq:
Leh_func_begin1:
Lfunc_begin1:
Ltmp0:
Ltmp1:
Ltmp2:
	movq	%rcx, %rax
Ltmp3:
Ltmp4:
Ltmp5:
Ltmp6:
Ltmp7:
Ltmp8:
	movb	3(%rdi), %r9b
	movb	4(%rdi), %cl
	movl	%esi, %r10d
	shrl	%cl, %r10d
	movzbl	1(%rdi), %ecx
	andl	%r10d, %ecx
	cvtsi2ss	%ecx, %xmm0
	movss	LCPI1_0(%rip), %xmm1
Ltmp9:
	mulss	%xmm0, %xmm1
	movb	%r9b, %cl
Ltmp10:
	movl	%esi, %r9d
	shrl	%cl, %r9d
	movzbl	(%rdi), %ecx
	andl	%r9d, %ecx
	cvtsi2ss	%ecx, %xmm2
	movss	LCPI1_1(%rip), %xmm3
Ltmp11:
	mulss	%xmm2, %xmm3
	addss	%xmm1, %xmm3
Ltmp12:
	movb	5(%rdi), %cl
	shrl	%cl, %esi
	movzbl	2(%rdi), %ecx
	andl	%esi, %ecx
	cvtsi2ss	%ecx, %xmm1
	movss	LCPI1_2(%rip), %xmm4
Ltmp13:
	mulss	%xmm1, %xmm4
	addss	%xmm3, %xmm4
	movss	%xmm4, (%rdx)
	movss	LCPI1_3(%rip), %xmm3
Ltmp14:
	mulss	%xmm0, %xmm3
	movss	LCPI1_4(%rip), %xmm4
	mulss	%xmm2, %xmm4
	addss	%xmm3, %xmm4
	movss	LCPI1_5(%rip), %xmm3
	mulss	%xmm1, %xmm3
	addss	%xmm4, %xmm3
	movss	%xmm3, (%rax)
Ltmp15:
	mulss	LCPI1_6(%rip), %xmm0
	mulss	LCPI1_7(%rip), %xmm2
	addss	%xmm0, %xmm2
	mulss	LCPI1_8(%rip), %xmm1
	addss	%xmm2, %xmm1
	movss	%xmm1, (%r8)
Ltmp16:
	ret
Ltmp17:
Lfunc_end1:
Leh_func_end1:

	.section	__TEXT,__literal4,4byte_literals
	.align	2
LCPI2_0:
	.long	1064615018
LCPI2_1:
	.long	1058994651
LCPI2_2:
	.long	1106771968
LCPI2_3:
	.long	3196797846
LCPI2_4:
	.long	3206914507
LCPI2_5:
	.long	3213717668
LCPI2_6:
	.long	1071242019
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_tv4x_yiq_to_rgb_unpacked
	.align	4, 0x90
_tv4x_yiq_to_rgb_unpacked:
Leh_func_begin2:
Lfunc_begin2:
Ltmp18:
Ltmp19:
Ltmp20:
Ltmp21:
Ltmp22:
Ltmp23:
Ltmp24:
	movss	LCPI2_0(%rip), %xmm3
Ltmp25:
	mulss	%xmm1, %xmm3
	addss	%xmm0, %xmm3
	movss	LCPI2_1(%rip), %xmm4
	mulss	%xmm2, %xmm4
	addss	%xmm3, %xmm4
Ltmp26:
	xorps	%xmm3, %xmm3
	movss	%xmm4, %xmm3
	movss	LCPI2_2(%rip), %xmm4
	minps	%xmm3, %xmm4
	xorps	%xmm3, %xmm3
	minps	%xmm4, %xmm3
Ltmp27:
Ltmp28:
	cvttss2si	%xmm3, %eax
	movb	%al, (%rsi)
	movss	LCPI2_3(%rip), %xmm3
Ltmp29:
	mulss	%xmm1, %xmm3
	addss	%xmm0, %xmm3
	movss	LCPI2_4(%rip), %xmm4
	mulss	%xmm2, %xmm4
	addss	%xmm3, %xmm4
Ltmp30:
Ltmp31:
	cvttss2si	%xmm4, %eax
	movb	%al, (%rdx)
Ltmp32:
	mulss	LCPI2_5(%rip), %xmm1
	addss	%xmm0, %xmm1
	mulss	LCPI2_6(%rip), %xmm2
	addss	%xmm1, %xmm2
Ltmp33:
Ltmp34:
	cvttss2si	%xmm2, %eax
	movb	%al, (%rcx)
Ltmp35:
	ret
Ltmp36:
Lfunc_end2:
Leh_func_end2:

	.section	__TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame0:
Lsection_eh_frame:
Leh_frame_common:
Lset0 = Leh_frame_common_end-Leh_frame_common_begin
	.long	Lset0
Leh_frame_common_begin:
	.long	0
	.byte	1
	.asciz	 "zR"
	.byte	1
	.byte	120
	.byte	16
	.byte	1
	.byte	16
	.byte	12
	.byte	7
	.byte	8
	.byte	144
	.byte	1
	.align	3
Leh_frame_common_end:
	.globl	_tv4x_rgb_to_yiq.eh
_tv4x_rgb_to_yiq.eh:
Lset1 = Leh_frame_end1-Leh_frame_begin1
	.long	Lset1
Leh_frame_begin1:
Lset2 = Leh_frame_begin1-Leh_frame_common
	.long	Lset2
Ltmp37:
	.quad	Leh_func_begin1-Ltmp37
Lset3 = Leh_func_end1-Leh_func_begin1
	.quad	Lset3
	.byte	0
	.align	3
Leh_frame_end1:

	.globl	_tv4x_yiq_to_rgb_unpacked.eh
_tv4x_yiq_to_rgb_unpacked.eh:
Lset4 = Leh_frame_end2-Leh_frame_begin2
	.long	Lset4
Leh_frame_begin2:
Lset5 = Leh_frame_begin2-Leh_frame_common
	.long	Lset5
Ltmp38:
	.quad	Leh_func_begin2-Ltmp38
Lset6 = Leh_func_end2-Leh_func_begin2
	.quad	Lset6
	.byte	0
	.align	3
Leh_frame_end2:

	.section	__TEXT,__text,regular,pure_instructions
Ltext_end:
	.section	__DATA,__data
Ldata_end:
	.section	__TEXT,__text,regular,pure_instructions
Lsection_end1:
	.section	__DWARF,__debug_frame,regular,debug
Ldebug_frame_common:
Lset7 = Ldebug_frame_common_end-Ldebug_frame_common_begin
	.long	Lset7
Ldebug_frame_common_begin:
	.long	4294967295
	.byte	1
	.byte	0
	.byte	1
	.byte	120
	.byte	16
	.byte	12
	.byte	7
	.byte	8
	.byte	144
	.byte	1
	.align	2
Ldebug_frame_common_end:
Lset8 = Ldebug_frame_end1-Ldebug_frame_begin1
	.long	Lset8
Ldebug_frame_begin1:
Lset9 = Ldebug_frame_common-Lsection_debug_frame
	.long	Lset9
	.quad	Lfunc_begin1
Lset10 = Lfunc_end1-Lfunc_begin1
	.quad	Lset10
	.align	2
Ldebug_frame_end1:
Lset11 = Ldebug_frame_end2-Ldebug_frame_begin2
	.long	Lset11
Ldebug_frame_begin2:
Lset12 = Ldebug_frame_common-Lsection_debug_frame
	.long	Lset12
	.quad	Lfunc_begin2
Lset13 = Lfunc_end2-Lfunc_begin2
	.quad	Lset13
	.align	2
Ldebug_frame_end2:
	.section	__DWARF,__debug_info,regular,debug
Linfo_begin1:
	.long	855
	.short	2
Lset14 = Labbrev_begin-Lsection_abbrev
	.long	Lset14
	.byte	8
	.byte	1
	.ascii	 "4.2.1 (Based on Apple Inc. build 5658) (LLVM build 2336.1.00)"
	.byte	0
	.byte	1
	.ascii	 "yiq.c"
	.byte	0
	.quad	0
	.long	0
	.ascii	 "/Users/cleure/Development/Projects/TV4X/src/yiq"
	.byte	0
	.byte	1
	.byte	2
	.long	305
	.ascii	 "tv4x_rgb_to_yiq"
	.byte	0
	.ascii	 "tv4x_rgb_to_yiq"
	.byte	0
	.byte	1
	.byte	35
	.byte	1
	.byte	1
	.quad	Lfunc_begin1
	.quad	Lfunc_end1
	.byte	1
	.byte	87
	.byte	1
	.byte	3
	.ascii	 "fmt"
	.byte	0
	.byte	1
	.byte	31
	.long	756
Lset15 = Ldebug_loc-Lsection_loc
	.long	Lset15
	.byte	3
	.ascii	 "rgb"
	.byte	0
	.byte	1
	.byte	32
	.long	780
Lset16 = Ldebug_loc3-Lsection_loc
	.long	Lset16
	.byte	3
	.byte	121
	.byte	0
	.byte	1
	.byte	33
	.long	805
Lset17 = Ldebug_loc6-Lsection_loc
	.long	Lset17
	.byte	3
	.byte	105
	.byte	0
	.byte	1
	.byte	34
	.long	805
Lset18 = Ldebug_loc9-Lsection_loc
	.long	Lset18
	.byte	3
	.byte	113
	.byte	0
	.byte	1
	.byte	35
	.long	805
Lset19 = Ldebug_loc12-Lsection_loc
	.long	Lset19
	.byte	4
	.long	0
	.byte	5
	.byte	114
	.byte	0
	.byte	1
	.byte	37
	.long	796
	.byte	5
	.byte	103
	.byte	0
	.byte	1
	.byte	37
	.long	796
	.byte	5
	.byte	98
	.byte	0
	.byte	1
	.byte	37
	.long	796
	.byte	0
	.byte	0
	.byte	2
	.long	594
	.ascii	 "tv4x_yiq_to_rgb_unpacked"
	.byte	0
	.ascii	 "tv4x_yiq_to_rgb_unpacked"
	.byte	0
	.byte	1
	.byte	80
	.byte	1
	.byte	1
	.quad	Lfunc_begin2
	.quad	Lfunc_end2
	.byte	1
	.byte	87
	.byte	1
	.byte	6
	.ascii	 "fmt"
	.byte	0
	.byte	1
	.byte	74
	.long	756
	.byte	1
	.byte	85
	.byte	3
	.ascii	 "ro"
	.byte	0
	.byte	1
	.byte	75
	.long	813
Lset20 = Ldebug_loc15-Lsection_loc
	.long	Lset20
	.byte	3
	.ascii	 "go"
	.byte	0
	.byte	1
	.byte	76
	.long	813
Lset21 = Ldebug_loc18-Lsection_loc
	.long	Lset21
	.byte	3
	.ascii	 "bo"
	.byte	0
	.byte	1
	.byte	77
	.long	813
Lset22 = Ldebug_loc21-Lsection_loc
	.long	Lset22
	.byte	3
	.byte	121
	.byte	0
	.byte	1
	.byte	78
	.long	796
Lset23 = Ldebug_loc24-Lsection_loc
	.long	Lset23
	.byte	3
	.byte	105
	.byte	0
	.byte	1
	.byte	79
	.long	796
Lset24 = Ldebug_loc27-Lsection_loc
	.long	Lset24
	.byte	3
	.byte	113
	.byte	0
	.byte	1
	.byte	80
	.long	796
Lset25 = Ldebug_loc30-Lsection_loc
	.long	Lset25
	.byte	7
	.quad	Ltmp24
	.quad	Ltmp36
	.byte	8
	.byte	114
	.byte	0
	.byte	1
	.byte	82
	.long	796
Lset26 = Ldebug_loc33-Lsection_loc
	.long	Lset26
	.byte	9
	.byte	103
	.byte	0
	.byte	1
	.byte	82
	.long	796
	.byte	1
	.byte	101
	.quad	Ltmp30
	.byte	9
	.byte	98
	.byte	0
	.byte	1
	.byte	82
	.long	796
	.byte	1
	.byte	99
	.quad	Ltmp33
	.byte	5
	.ascii	 "maxclamp"
	.byte	0
	.byte	1
	.byte	83
	.long	840
	.byte	5
	.ascii	 "minclamp"
	.byte	0
	.byte	1
	.byte	84
	.long	840
	.byte	5
	.ascii	 "clamped"
	.byte	0
	.byte	1
	.byte	85
	.long	840
	.byte	0
	.byte	0
	.byte	10
	.byte	8
	.ascii	 "unsigned char"
	.byte	0
	.byte	1
	.byte	11
	.long	594
	.ascii	 "uint8_t"
	.byte	0
	.byte	1
	.byte	45
	.byte	12
	.long	756
	.ascii	 "tv4x_rgb_format"
	.byte	0
	.byte	6
	.byte	1
	.byte	55
	.byte	13
	.ascii	 "r_mask"
	.byte	0
	.long	611
	.byte	1
	.byte	24
	.byte	2
	.byte	35
	.byte	0
	.byte	13
	.ascii	 "g_mask"
	.byte	0
	.long	611
	.byte	1
	.byte	25
	.byte	2
	.byte	35
	.byte	1
	.byte	13
	.ascii	 "b_mask"
	.byte	0
	.long	611
	.byte	1
	.byte	26
	.byte	2
	.byte	35
	.byte	2
	.byte	13
	.ascii	 "r_shift"
	.byte	0
	.long	611
	.byte	1
	.byte	28
	.byte	2
	.byte	35
	.byte	3
	.byte	13
	.ascii	 "g_shift"
	.byte	0
	.long	611
	.byte	1
	.byte	29
	.byte	2
	.byte	35
	.byte	4
	.byte	13
	.ascii	 "b_shift"
	.byte	0
	.long	611
	.byte	1
	.byte	30
	.byte	2
	.byte	35
	.byte	5
	.byte	0
	.byte	14
	.long	626
	.byte	8
	.byte	1
	.byte	0
	.byte	10
	.byte	7
	.ascii	 "unsigned int"
	.byte	0
	.byte	4
	.byte	11
	.long	764
	.ascii	 "uint32_t"
	.byte	0
	.byte	1
	.byte	55
	.byte	10
	.byte	4
	.ascii	 "float"
	.byte	0
	.byte	4
	.byte	14
	.long	796
	.byte	8
	.byte	1
	.byte	0
	.byte	14
	.long	611
	.byte	8
	.byte	1
	.byte	0
	.byte	15
	.byte	4
	.byte	5
	.byte	16
	.long	840
	.long	796
	.byte	17
	.long	821
	.byte	3
	.byte	0
	.byte	11
	.long	824
	.ascii	 "__m128"
	.byte	0
	.byte	1
	.byte	54
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
Linfo_end1:
	.section	__DWARF,__debug_abbrev,regular,debug
Labbrev_begin:
	.byte	1
	.byte	17
	.byte	1
	.byte	37
	.byte	8
	.byte	19
	.byte	11
	.byte	3
	.byte	8
	.byte	82
	.byte	1
	.byte	16
	.byte	6
	.byte	27
	.byte	8
	.byte	225
	.byte	127
	.byte	12
	.byte	0
	.byte	0
	.byte	2
	.byte	46
	.byte	1
	.byte	1
	.byte	19
	.byte	3
	.byte	8
	.byte	135
	.byte	64
	.byte	8
	.byte	58
	.byte	11
	.byte	59
	.byte	11
	.byte	39
	.byte	12
	.byte	63
	.byte	12
	.byte	17
	.byte	1
	.byte	18
	.byte	1
	.byte	64
	.byte	10
	.byte	231
	.byte	127
	.byte	12
	.byte	0
	.byte	0
	.byte	3
	.byte	5
	.byte	0
	.byte	3
	.byte	8
	.byte	58
	.byte	11
	.byte	59
	.byte	11
	.byte	73
	.byte	19
	.byte	2
	.byte	6
	.byte	0
	.byte	0
	.byte	4
	.byte	11
	.byte	1
	.byte	85
	.byte	6
	.byte	0
	.byte	0
	.byte	5
	.byte	52
	.byte	0
	.byte	3
	.byte	8
	.byte	58
	.byte	11
	.byte	59
	.byte	11
	.byte	73
	.byte	19
	.byte	0
	.byte	0
	.byte	6
	.byte	5
	.byte	0
	.byte	3
	.byte	8
	.byte	58
	.byte	11
	.byte	59
	.byte	11
	.byte	73
	.byte	19
	.byte	2
	.byte	10
	.byte	0
	.byte	0
	.byte	7
	.byte	11
	.byte	1
	.byte	17
	.byte	1
	.byte	18
	.byte	1
	.byte	0
	.byte	0
	.byte	8
	.byte	52
	.byte	0
	.byte	3
	.byte	8
	.byte	58
	.byte	11
	.byte	59
	.byte	11
	.byte	73
	.byte	19
	.byte	2
	.byte	6
	.byte	0
	.byte	0
	.byte	9
	.byte	52
	.byte	0
	.byte	3
	.byte	8
	.byte	58
	.byte	11
	.byte	59
	.byte	11
	.byte	73
	.byte	19
	.byte	2
	.byte	10
	.byte	44
	.byte	1
	.byte	0
	.byte	0
	.byte	10
	.byte	36
	.byte	0
	.byte	62
	.byte	11
	.byte	3
	.byte	8
	.byte	11
	.byte	11
	.byte	0
	.byte	0
	.byte	11
	.byte	22
	.byte	0
	.byte	73
	.byte	19
	.byte	3
	.byte	8
	.byte	58
	.byte	11
	.byte	59
	.byte	11
	.byte	0
	.byte	0
	.byte	12
	.byte	19
	.byte	1
	.byte	1
	.byte	19
	.byte	3
	.byte	8
	.byte	11
	.byte	11
	.byte	58
	.byte	11
	.byte	59
	.byte	11
	.byte	0
	.byte	0
	.byte	13
	.byte	13
	.byte	0
	.byte	3
	.byte	8
	.byte	73
	.byte	19
	.byte	58
	.byte	11
	.byte	59
	.byte	11
	.byte	56
	.byte	10
	.byte	0
	.byte	0
	.byte	14
	.byte	15
	.byte	0
	.byte	73
	.byte	19
	.byte	11
	.byte	11
	.byte	58
	.byte	11
	.byte	59
	.byte	11
	.byte	0
	.byte	0
	.byte	15
	.byte	36
	.byte	0
	.byte	11
	.byte	11
	.byte	62
	.byte	11
	.byte	0
	.byte	0
	.byte	16
	.byte	1
	.byte	1
	.byte	1
	.byte	19
	.byte	73
	.byte	19
	.byte	0
	.byte	0
	.byte	17
	.byte	33
	.byte	0
	.byte	73
	.byte	19
	.byte	47
	.byte	11
	.byte	0
	.byte	0
	.byte	0
Labbrev_end:
	.section	__DWARF,__debug_line,regular,debug
Lset27 = Lline_end-Lline_begin
	.long	Lset27
Lline_begin:
	.short	2
Lset28 = Lline_prolog_end-Lline_prolog_begin
	.long	Lset28
Lline_prolog_begin:
	.byte	1
	.byte	1
	.byte	246
	.byte	245
	.byte	10
	.byte	0
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.asciz	 "/Users/cleure/Development/Projects/TV4X/src/yiq"
	.byte	0
	.asciz	 "yiq.c"
	.byte	1
	.byte	0
	.byte	0
	.byte	0
Lline_prolog_end:
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp0
	.byte	54
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp1
	.byte	1
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp2
	.byte	24
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp4
	.byte	15
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp5
	.byte	19
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp6
	.byte	19
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp7
	.byte	19
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp8
	.byte	28
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp9
	.byte	29
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp10
	.byte	11
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp11
	.byte	29
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp12
	.byte	11
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp13
	.byte	29
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp14
	.byte	24
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp15
	.byte	24
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp16
	.byte	23
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp18
	.byte	41
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp19
	.byte	1
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp20
	.byte	19
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp21
	.byte	19
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp22
	.byte	19
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp23
	.byte	19
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp24
	.byte	19
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp25
	.byte	33
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp26
	.byte	1
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp27
	.byte	67
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp28
	.byte	28
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp29
	.byte	3
	.byte	78
	.byte	1
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp30
	.byte	1
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp31
	.byte	71
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp32
	.byte	3
	.byte	82
	.byte	1
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp33
	.byte	1
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp34
	.byte	67
	.byte	0
	.byte	9
	.byte	2
	.quad	Ltmp35
	.byte	21
	.byte	0
	.byte	9
	.byte	2
	.quad	Lsection_end1
	.byte	0
	.byte	1
	.byte	1
Lline_end:
	.section	__DWARF,__debug_pubnames,regular,debug
Lset29 = Lpubnames_end1-Lpubnames_begin1
	.long	Lset29
Lpubnames_begin1:
	.short	2
Lset30 = Linfo_begin1-Lsection_info
	.long	Lset30
Lset31 = Linfo_end1-Linfo_begin1
	.long	Lset31
	.long	142
	.asciz	 "tv4x_rgb_to_yiq"
	.long	305
	.asciz	 "tv4x_yiq_to_rgb_unpacked"
	.long	0
Lpubnames_end1:
	.section	__DWARF,__debug_pubtypes,regular,debug
Lset32 = Lpubtypes_end1-Lpubtypes_begin1
	.long	Lset32
Lpubtypes_begin1:
	.short	2
Lset33 = Linfo_begin1-Lsection_info
	.long	Lset33
Lset34 = Linfo_end1-Linfo_begin1
	.long	Lset34
	.long	626
	.asciz	 "tv4x_rgb_format"
	.long	0
Lpubtypes_end1:
	.section	__DWARF,__debug_loc,regular,debug
Ldebug_loc:
	.quad	Lfunc_begin1
	.quad	Ltmp7
	.short	1
	.byte	85
	.quad	Ltmp7
	.quad	Lfunc_end1
	.short	1
	.byte	85
	.quad	0
	.quad	0
Ldebug_loc3:
	.quad	Lfunc_begin1
	.quad	Ltmp6
	.short	1
	.byte	84
	.quad	Ltmp6
	.quad	Lfunc_end1
	.short	1
	.byte	84
	.quad	0
	.quad	0
Ldebug_loc6:
	.quad	Lfunc_begin1
	.quad	Ltmp5
	.short	1
	.byte	81
	.quad	Ltmp5
	.quad	Lfunc_end1
	.short	1
	.byte	81
	.quad	0
	.quad	0
Ldebug_loc9:
	.quad	Lfunc_begin1
	.quad	Ltmp4
	.short	1
	.byte	82
	.quad	Ltmp4
	.quad	Lfunc_end1
	.short	1
	.byte	80
	.quad	0
	.quad	0
Ldebug_loc12:
	.quad	Lfunc_begin1
	.quad	Ltmp1
	.short	1
	.byte	88
	.quad	Ltmp1
	.quad	Lfunc_end1
	.short	1
	.byte	88
	.quad	0
	.quad	0
Ldebug_loc15:
	.quad	Lfunc_begin2
	.quad	Ltmp24
	.short	1
	.byte	84
	.quad	Ltmp24
	.quad	Lfunc_end2
	.short	1
	.byte	84
	.quad	0
	.quad	0
Ldebug_loc18:
	.quad	Lfunc_begin2
	.quad	Ltmp23
	.short	1
	.byte	81
	.quad	Ltmp23
	.quad	Lfunc_end2
	.short	1
	.byte	81
	.quad	0
	.quad	0
Ldebug_loc21:
	.quad	Lfunc_begin2
	.quad	Ltmp22
	.short	1
	.byte	82
	.quad	Ltmp22
	.quad	Lfunc_end2
	.short	1
	.byte	82
	.quad	0
	.quad	0
Ldebug_loc24:
	.quad	Lfunc_begin2
	.quad	Ltmp21
	.short	1
	.byte	97
	.quad	Ltmp21
	.quad	Lfunc_end2
	.short	1
	.byte	97
	.quad	0
	.quad	0
Ldebug_loc27:
	.quad	Lfunc_begin2
	.quad	Ltmp20
	.short	1
	.byte	98
	.quad	Ltmp20
	.quad	Lfunc_end2
	.short	1
	.byte	98
	.quad	0
	.quad	0
Ldebug_loc30:
	.quad	Lfunc_begin2
	.quad	Ltmp19
	.short	1
	.byte	99
	.quad	Ltmp19
	.quad	Lfunc_end2
	.short	1
	.byte	99
	.quad	0
	.quad	0
Ldebug_loc33:
	.quad	Ltmp26
	.quad	Ltmp27
	.short	1
	.byte	101
	.quad	Ltmp27
	.quad	Lfunc_end2
	.short	1
	.byte	100
	.quad	0
	.quad	0
Ldebug_loc36:
	.section	__DWARF,__debug_aranges,regular,debug
	.section	__DWARF,__debug_ranges,regular,debug
	.quad	Ltmp2
	.quad	Ltmp3
	.quad	Ltmp8
	.quad	Ltmp17
	.quad	0
	.quad	0
	.section	__DWARF,__debug_macinfo,regular,debug
	.section	__DWARF,__debug_inlined,regular,debug
Lset35 = Ldebug_inlined_end1-Ldebug_inlined_begin1
	.long	Lset35
Ldebug_inlined_begin1:
	.short	2
	.byte	8
Ldebug_inlined_end1:

.subsections_via_symbols
