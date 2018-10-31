	.file	1 "asm-offsets.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3

 # -G value = 0, Arch = mips32r2, ISA = 33
 # GNU C (Buildroot 2012.11.1) version 4.6.3 (mipsel-buildroot-linux-uclibc)
 #	compiled by GNU C version 4.4.3, GMP version 5.0.5, MPFR version 3.1.1, MPC version 1.0.1
 # GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
 # options passed:  -nostdinc
 # -I /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include
 # -I include
 # -I /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-ralink
 # -I /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-generic
 # -D __KERNEL__ -D VMLINUX_LOAD_ADDRESS=0xffffffff80000000 -D DATAOFFSET=0
 # -D CONFIG_MIPS_TC3262 -D KBUILD_STR(s)=#s
 # -D KBUILD_BASENAME=KBUILD_STR(asm_offsets)
 # -D KBUILD_MODNAME=KBUILD_STR(asm_offsets)
 # -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include
 # -include include/generated/autoconf.h
 # -MD arch/mips/kernel/.asm-offsets.s.d arch/mips/kernel/asm-offsets.c
 # -G 0 -mno-check-zero-division -mabi=32 -mno-abicalls -msoft-float
 # -march=mips32r2 -mtune=mips32r2 -mllsc -mno-synci -mno-shared
 # -auxbase-strip arch/mips/kernel/asm-offsets.s -O2 -Wall -Wundef
 # -Wstrict-prototypes -Wno-trigraphs -Werror=implicit-function-declaration
 # -Wno-format-security -Wframe-larger-than=1024
 # -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-aliasing
 # -fno-common -fno-delete-null-pointer-checks -ffunction-sections -fno-pic
 # -ffreestanding -fno-stack-protector -fomit-frame-pointer
 # -fno-strict-overflow -fconserve-stack -fverbose-asm
 # options enabled:  -fauto-inc-dec -fbranch-count-reg -fcaller-saves
 # -fcombine-stack-adjustments -fcompare-elim -fcprop-registers
 # -fcrossjumping -fcse-follow-jumps -fdefer-pop -fdelayed-branch
 # -fdevirtualize -fdwarf2-cfi-asm -fearly-inlining
 # -feliminate-unused-debug-types -fexpensive-optimizations
 # -fforward-propagate -ffunction-cse -ffunction-sections -fgcse -fgcse-lm
 # -fguess-branch-probability -fident -fif-conversion -fif-conversion2
 # -findirect-inlining -finline -finline-functions-called-once
 # -finline-small-functions -fipa-cp -fipa-profile -fipa-pure-const
 # -fipa-reference -fipa-sra -fira-share-save-slots -fira-share-spill-slots
 # -fivopts -fkeep-static-consts -fleading-underscore -fmath-errno
 # -fmerge-constants -fmerge-debug-strings -fmove-loop-invariants
 # -fomit-frame-pointer -foptimize-register-move -foptimize-sibling-calls
 # -fpartial-inlining -fpcc-struct-return -fpeephole -fpeephole2
 # -fprefetch-loop-arrays -fregmove -freorder-blocks -freorder-functions
 # -frerun-cse-after-loop -fsched-critical-path-heuristic
 # -fsched-dep-count-heuristic -fsched-group-heuristic -fsched-interblock
 # -fsched-last-insn-heuristic -fsched-rank-heuristic -fsched-spec
 # -fsched-spec-insn-heuristic -fsched-stalled-insns-dep -fschedule-insns
 # -fschedule-insns2 -fshow-column -fsigned-zeros -fsplit-ivs-in-unroller
 # -fsplit-wide-types -fstrict-volatile-bitfields -fthread-jumps
 # -ftoplevel-reorder -ftrapping-math -ftree-bit-ccp
 # -ftree-builtin-call-dce -ftree-ccp -ftree-ch -ftree-copy-prop
 # -ftree-copyrename -ftree-cselim -ftree-dce -ftree-dominator-opts
 # -ftree-dse -ftree-forwprop -ftree-fre -ftree-loop-if-convert
 # -ftree-loop-im -ftree-loop-ivcanon -ftree-loop-optimize
 # -ftree-parallelize-loops= -ftree-phiprop -ftree-pre -ftree-pta
 # -ftree-reassoc -ftree-scev-cprop -ftree-sink -ftree-slp-vectorize
 # -ftree-sra -ftree-switch-conversion -ftree-ter -ftree-vect-loop-version
 # -ftree-vrp -funit-at-a-time -fverbose-asm -fzero-initialized-in-bss
 # -mdivide-traps -mdouble-float -mel -mexplicit-relocs -mextern-sdata
 # -mfp-exceptions -mfp32 -mfused-madd -mgp32 -mgpopt -mllsc -mlocal-sdata
 # -mlong32 -mno-mdmx -mno-mips16 -mno-mips3d -msoft-float
 # -msplit-addresses -muclibc

 # Compiler executable checksum: 803a9f797010d160a1bef2d33c25729e

#APP
	.macro _ssnop; sll $0, $0, 1; .endm
	.macro _ehb; sll $0, $0, 3; .endm
	.macro mtc0_tlbw_hazard; _ehb; .endm
	.macro tlbw_use_hazard; _ehb; .endm
	.macro tlb_probe_hazard; _ehb; .endm
	.macro irq_enable_hazard; _ehb; .endm
	.macro irq_disable_hazard; _ehb; .endm
	.macro back_to_back_c0_hazard; _ehb; .endm
	.macro enable_fpu_hazard; _ehb; .endm
	.macro disable_fpu_hazard; _ehb; .endm
		.macro	raw_local_irq_enable				
	.set	push						
	.set	reorder						
	.set	noat						
	ei							
	irq_enable_hazard					
	.set	pop						
	.endm
		.macro	raw_local_irq_disable
	.set	push						
	.set	noat						
	di							
	irq_disable_hazard					
	.set	pop						
	.endm							

		.macro	raw_local_save_flags flags			
	.set	push						
	.set	reorder						
	mfc0	\flags, $12					
	.set	pop						
	.endm							

		.macro	raw_local_irq_save result			
	.set	push						
	.set	reorder						
	.set	noat						
	di	\result					
	andi	\result, 1					
	irq_disable_hazard					
	.set	pop						
	.endm							

		.macro	raw_local_irq_restore flags			
	.set	push						
	.set	noreorder					
	.set	noat						
	beqz	\flags, 1f					
	 di							
	ei							
1:								
	irq_disable_hazard					
	.set	pop						
	.endm							

#NO_APP
	.section	.text.output_ptreg_defines,"ax",@progbits
	.align	2
	.globl	output_ptreg_defines
	.set	nomips16
	.ent	output_ptreg_defines
	.type	output_ptreg_defines, @function
output_ptreg_defines:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
#APP
 # 23 "arch/mips/kernel/asm-offsets.c" 1
	
->#MIPS pt_regs offsets.
 # 0 "" 2
 # 24 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R0 24 offsetof(struct pt_regs, regs[0])	 #
 # 0 "" 2
 # 25 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R1 28 offsetof(struct pt_regs, regs[1])	 #
 # 0 "" 2
 # 26 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R2 32 offsetof(struct pt_regs, regs[2])	 #
 # 0 "" 2
 # 27 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R3 36 offsetof(struct pt_regs, regs[3])	 #
 # 0 "" 2
 # 28 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R4 40 offsetof(struct pt_regs, regs[4])	 #
 # 0 "" 2
 # 29 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R5 44 offsetof(struct pt_regs, regs[5])	 #
 # 0 "" 2
 # 30 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R6 48 offsetof(struct pt_regs, regs[6])	 #
 # 0 "" 2
 # 31 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R7 52 offsetof(struct pt_regs, regs[7])	 #
 # 0 "" 2
 # 32 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R8 56 offsetof(struct pt_regs, regs[8])	 #
 # 0 "" 2
 # 33 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R9 60 offsetof(struct pt_regs, regs[9])	 #
 # 0 "" 2
 # 34 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R10 64 offsetof(struct pt_regs, regs[10])	 #
 # 0 "" 2
 # 35 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R11 68 offsetof(struct pt_regs, regs[11])	 #
 # 0 "" 2
 # 36 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R12 72 offsetof(struct pt_regs, regs[12])	 #
 # 0 "" 2
 # 37 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R13 76 offsetof(struct pt_regs, regs[13])	 #
 # 0 "" 2
 # 38 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R14 80 offsetof(struct pt_regs, regs[14])	 #
 # 0 "" 2
 # 39 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R15 84 offsetof(struct pt_regs, regs[15])	 #
 # 0 "" 2
 # 40 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R16 88 offsetof(struct pt_regs, regs[16])	 #
 # 0 "" 2
 # 41 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R17 92 offsetof(struct pt_regs, regs[17])	 #
 # 0 "" 2
 # 42 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R18 96 offsetof(struct pt_regs, regs[18])	 #
 # 0 "" 2
 # 43 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R19 100 offsetof(struct pt_regs, regs[19])	 #
 # 0 "" 2
 # 44 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R20 104 offsetof(struct pt_regs, regs[20])	 #
 # 0 "" 2
 # 45 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R21 108 offsetof(struct pt_regs, regs[21])	 #
 # 0 "" 2
 # 46 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R22 112 offsetof(struct pt_regs, regs[22])	 #
 # 0 "" 2
 # 47 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R23 116 offsetof(struct pt_regs, regs[23])	 #
 # 0 "" 2
 # 48 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R24 120 offsetof(struct pt_regs, regs[24])	 #
 # 0 "" 2
 # 49 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R25 124 offsetof(struct pt_regs, regs[25])	 #
 # 0 "" 2
 # 50 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R26 128 offsetof(struct pt_regs, regs[26])	 #
 # 0 "" 2
 # 51 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R27 132 offsetof(struct pt_regs, regs[27])	 #
 # 0 "" 2
 # 52 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R28 136 offsetof(struct pt_regs, regs[28])	 #
 # 0 "" 2
 # 53 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R29 140 offsetof(struct pt_regs, regs[29])	 #
 # 0 "" 2
 # 54 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R30 144 offsetof(struct pt_regs, regs[30])	 #
 # 0 "" 2
 # 55 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_R31 148 offsetof(struct pt_regs, regs[31])	 #
 # 0 "" 2
 # 56 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_LO 160 offsetof(struct pt_regs, lo)	 #
 # 0 "" 2
 # 57 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_HI 156 offsetof(struct pt_regs, hi)	 #
 # 0 "" 2
 # 61 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_EPC 172 offsetof(struct pt_regs, cp0_epc)	 #
 # 0 "" 2
 # 62 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_BVADDR 164 offsetof(struct pt_regs, cp0_badvaddr)	 #
 # 0 "" 2
 # 63 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_STATUS 152 offsetof(struct pt_regs, cp0_status)	 #
 # 0 "" 2
 # 64 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_CAUSE 168 offsetof(struct pt_regs, cp0_cause)	 #
 # 0 "" 2
 # 72 "arch/mips/kernel/asm-offsets.c" 1
	
->PT_SIZE 176 sizeof(struct pt_regs)	 #
 # 0 "" 2
 # 73 "arch/mips/kernel/asm-offsets.c" 1
	
->
 # 0 "" 2
#NO_APP
	j	$31
	.end	output_ptreg_defines
	.size	output_ptreg_defines, .-output_ptreg_defines
	.section	.text.output_task_defines,"ax",@progbits
	.align	2
	.globl	output_task_defines
	.set	nomips16
	.ent	output_task_defines
	.type	output_task_defines, @function
output_task_defines:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
#APP
 # 78 "arch/mips/kernel/asm-offsets.c" 1
	
->#MIPS task_struct offsets.
 # 0 "" 2
 # 79 "arch/mips/kernel/asm-offsets.c" 1
	
->TASK_STATE 0 offsetof(struct task_struct, state)	 #
 # 0 "" 2
 # 80 "arch/mips/kernel/asm-offsets.c" 1
	
->TASK_THREAD_INFO 4 offsetof(struct task_struct, stack)	 #
 # 0 "" 2
 # 81 "arch/mips/kernel/asm-offsets.c" 1
	
->TASK_FLAGS 12 offsetof(struct task_struct, flags)	 #
 # 0 "" 2
 # 82 "arch/mips/kernel/asm-offsets.c" 1
	
->TASK_MM 184 offsetof(struct task_struct, mm)	 #
 # 0 "" 2
 # 83 "arch/mips/kernel/asm-offsets.c" 1
	
->TASK_PID 216 offsetof(struct task_struct, pid)	 #
 # 0 "" 2
 # 84 "arch/mips/kernel/asm-offsets.c" 1
	
->TASK_STRUCT_SIZE 1112 sizeof(struct task_struct)	 #
 # 0 "" 2
 # 85 "arch/mips/kernel/asm-offsets.c" 1
	
->
 # 0 "" 2
#NO_APP
	j	$31
	.end	output_task_defines
	.size	output_task_defines, .-output_task_defines
	.section	.text.output_thread_info_defines,"ax",@progbits
	.align	2
	.globl	output_thread_info_defines
	.set	nomips16
	.ent	output_thread_info_defines
	.type	output_thread_info_defines, @function
output_thread_info_defines:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
#APP
 # 90 "arch/mips/kernel/asm-offsets.c" 1
	
->#MIPS thread_info offsets.
 # 0 "" 2
 # 91 "arch/mips/kernel/asm-offsets.c" 1
	
->TI_TASK 0 offsetof(struct thread_info, task)	 #
 # 0 "" 2
 # 92 "arch/mips/kernel/asm-offsets.c" 1
	
->TI_EXEC_DOMAIN 4 offsetof(struct thread_info, exec_domain)	 #
 # 0 "" 2
 # 93 "arch/mips/kernel/asm-offsets.c" 1
	
->TI_FLAGS 8 offsetof(struct thread_info, flags)	 #
 # 0 "" 2
 # 94 "arch/mips/kernel/asm-offsets.c" 1
	
->TI_TP_VALUE 12 offsetof(struct thread_info, tp_value)	 #
 # 0 "" 2
 # 95 "arch/mips/kernel/asm-offsets.c" 1
	
->TI_CPU 16 offsetof(struct thread_info, cpu)	 #
 # 0 "" 2
 # 96 "arch/mips/kernel/asm-offsets.c" 1
	
->TI_PRE_COUNT 20 offsetof(struct thread_info, preempt_count)	 #
 # 0 "" 2
 # 97 "arch/mips/kernel/asm-offsets.c" 1
	
->TI_ADDR_LIMIT 24 offsetof(struct thread_info, addr_limit)	 #
 # 0 "" 2
 # 98 "arch/mips/kernel/asm-offsets.c" 1
	
->TI_RESTART_BLOCK 32 offsetof(struct thread_info, restart_block)	 #
 # 0 "" 2
 # 99 "arch/mips/kernel/asm-offsets.c" 1
	
->TI_REGS 72 offsetof(struct thread_info, regs)	 #
 # 0 "" 2
 # 100 "arch/mips/kernel/asm-offsets.c" 1
	
->_THREAD_SIZE 8192 THREAD_SIZE	 #
 # 0 "" 2
 # 101 "arch/mips/kernel/asm-offsets.c" 1
	
->_THREAD_MASK 8191 THREAD_MASK	 #
 # 0 "" 2
 # 102 "arch/mips/kernel/asm-offsets.c" 1
	
->
 # 0 "" 2
#NO_APP
	j	$31
	.end	output_thread_info_defines
	.size	output_thread_info_defines, .-output_thread_info_defines
	.section	.text.output_thread_defines,"ax",@progbits
	.align	2
	.globl	output_thread_defines
	.set	nomips16
	.ent	output_thread_defines
	.type	output_thread_defines, @function
output_thread_defines:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
#APP
 # 107 "arch/mips/kernel/asm-offsets.c" 1
	
->#MIPS specific thread_struct offsets.
 # 0 "" 2
 # 108 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_REG16 480 offsetof(struct task_struct, thread.reg16)	 #
 # 0 "" 2
 # 109 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_REG17 484 offsetof(struct task_struct, thread.reg17)	 #
 # 0 "" 2
 # 110 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_REG18 488 offsetof(struct task_struct, thread.reg18)	 #
 # 0 "" 2
 # 111 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_REG19 492 offsetof(struct task_struct, thread.reg19)	 #
 # 0 "" 2
 # 112 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_REG20 496 offsetof(struct task_struct, thread.reg20)	 #
 # 0 "" 2
 # 113 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_REG21 500 offsetof(struct task_struct, thread.reg21)	 #
 # 0 "" 2
 # 114 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_REG22 504 offsetof(struct task_struct, thread.reg22)	 #
 # 0 "" 2
 # 115 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_REG23 508 offsetof(struct task_struct, thread.reg23)	 #
 # 0 "" 2
 # 116 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_REG29 512 offsetof(struct task_struct, thread.reg29)	 #
 # 0 "" 2
 # 117 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_REG30 516 offsetof(struct task_struct, thread.reg30)	 #
 # 0 "" 2
 # 118 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_REG31 520 offsetof(struct task_struct, thread.reg31)	 #
 # 0 "" 2
 # 119 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_STATUS 524 offsetof(struct task_struct, thread.cp0_status)	 #
 # 0 "" 2
 # 121 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPU 528 offsetof(struct task_struct, thread.fpu)	 #
 # 0 "" 2
 # 123 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_BVADDR 844 offsetof(struct task_struct, thread.cp0_badvaddr)	 #
 # 0 "" 2
 # 125 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_BUADDR 848 offsetof(struct task_struct, thread.cp0_baduaddr)	 #
 # 0 "" 2
 # 127 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_ECODE 852 offsetof(struct task_struct, thread.error_code)	 #
 # 0 "" 2
 # 129 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_TRAMP 856 offsetof(struct task_struct, thread.irix_trampoline)	 #
 # 0 "" 2
 # 131 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_OLDCTX 860 offsetof(struct task_struct, thread.irix_oldctx)	 #
 # 0 "" 2
 # 133 "arch/mips/kernel/asm-offsets.c" 1
	
->
 # 0 "" 2
#NO_APP
	j	$31
	.end	output_thread_defines
	.size	output_thread_defines, .-output_thread_defines
	.section	.text.output_thread_fpu_defines,"ax",@progbits
	.align	2
	.globl	output_thread_fpu_defines
	.set	nomips16
	.ent	output_thread_fpu_defines
	.type	output_thread_fpu_defines, @function
output_thread_fpu_defines:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
#APP
 # 138 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR0 528 offsetof(struct task_struct, thread.fpu.fpr[0])	 #
 # 0 "" 2
 # 139 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR1 536 offsetof(struct task_struct, thread.fpu.fpr[1])	 #
 # 0 "" 2
 # 140 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR2 544 offsetof(struct task_struct, thread.fpu.fpr[2])	 #
 # 0 "" 2
 # 141 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR3 552 offsetof(struct task_struct, thread.fpu.fpr[3])	 #
 # 0 "" 2
 # 142 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR4 560 offsetof(struct task_struct, thread.fpu.fpr[4])	 #
 # 0 "" 2
 # 143 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR5 568 offsetof(struct task_struct, thread.fpu.fpr[5])	 #
 # 0 "" 2
 # 144 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR6 576 offsetof(struct task_struct, thread.fpu.fpr[6])	 #
 # 0 "" 2
 # 145 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR7 584 offsetof(struct task_struct, thread.fpu.fpr[7])	 #
 # 0 "" 2
 # 146 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR8 592 offsetof(struct task_struct, thread.fpu.fpr[8])	 #
 # 0 "" 2
 # 147 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR9 600 offsetof(struct task_struct, thread.fpu.fpr[9])	 #
 # 0 "" 2
 # 148 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR10 608 offsetof(struct task_struct, thread.fpu.fpr[10])	 #
 # 0 "" 2
 # 149 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR11 616 offsetof(struct task_struct, thread.fpu.fpr[11])	 #
 # 0 "" 2
 # 150 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR12 624 offsetof(struct task_struct, thread.fpu.fpr[12])	 #
 # 0 "" 2
 # 151 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR13 632 offsetof(struct task_struct, thread.fpu.fpr[13])	 #
 # 0 "" 2
 # 152 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR14 640 offsetof(struct task_struct, thread.fpu.fpr[14])	 #
 # 0 "" 2
 # 153 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR15 648 offsetof(struct task_struct, thread.fpu.fpr[15])	 #
 # 0 "" 2
 # 154 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR16 656 offsetof(struct task_struct, thread.fpu.fpr[16])	 #
 # 0 "" 2
 # 155 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR17 664 offsetof(struct task_struct, thread.fpu.fpr[17])	 #
 # 0 "" 2
 # 156 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR18 672 offsetof(struct task_struct, thread.fpu.fpr[18])	 #
 # 0 "" 2
 # 157 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR19 680 offsetof(struct task_struct, thread.fpu.fpr[19])	 #
 # 0 "" 2
 # 158 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR20 688 offsetof(struct task_struct, thread.fpu.fpr[20])	 #
 # 0 "" 2
 # 159 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR21 696 offsetof(struct task_struct, thread.fpu.fpr[21])	 #
 # 0 "" 2
 # 160 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR22 704 offsetof(struct task_struct, thread.fpu.fpr[22])	 #
 # 0 "" 2
 # 161 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR23 712 offsetof(struct task_struct, thread.fpu.fpr[23])	 #
 # 0 "" 2
 # 162 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR24 720 offsetof(struct task_struct, thread.fpu.fpr[24])	 #
 # 0 "" 2
 # 163 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR25 728 offsetof(struct task_struct, thread.fpu.fpr[25])	 #
 # 0 "" 2
 # 164 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR26 736 offsetof(struct task_struct, thread.fpu.fpr[26])	 #
 # 0 "" 2
 # 165 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR27 744 offsetof(struct task_struct, thread.fpu.fpr[27])	 #
 # 0 "" 2
 # 166 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR28 752 offsetof(struct task_struct, thread.fpu.fpr[28])	 #
 # 0 "" 2
 # 167 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR29 760 offsetof(struct task_struct, thread.fpu.fpr[29])	 #
 # 0 "" 2
 # 168 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR30 768 offsetof(struct task_struct, thread.fpu.fpr[30])	 #
 # 0 "" 2
 # 169 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FPR31 776 offsetof(struct task_struct, thread.fpu.fpr[31])	 #
 # 0 "" 2
 # 171 "arch/mips/kernel/asm-offsets.c" 1
	
->THREAD_FCR31 784 offsetof(struct task_struct, thread.fpu.fcr31)	 #
 # 0 "" 2
 # 172 "arch/mips/kernel/asm-offsets.c" 1
	
->
 # 0 "" 2
#NO_APP
	j	$31
	.end	output_thread_fpu_defines
	.size	output_thread_fpu_defines, .-output_thread_fpu_defines
	.section	.text.output_mm_defines,"ax",@progbits
	.align	2
	.globl	output_mm_defines
	.set	nomips16
	.ent	output_mm_defines
	.type	output_mm_defines, @function
output_mm_defines:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
#APP
 # 177 "arch/mips/kernel/asm-offsets.c" 1
	
->#Size of struct page
 # 0 "" 2
 # 178 "arch/mips/kernel/asm-offsets.c" 1
	
->STRUCT_PAGE_SIZE 32 sizeof(struct page)	 #
 # 0 "" 2
 # 179 "arch/mips/kernel/asm-offsets.c" 1
	
->
 # 0 "" 2
 # 180 "arch/mips/kernel/asm-offsets.c" 1
	
->#Linux mm_struct offsets.
 # 0 "" 2
 # 181 "arch/mips/kernel/asm-offsets.c" 1
	
->MM_USERS 40 offsetof(struct mm_struct, mm_users)	 #
 # 0 "" 2
 # 182 "arch/mips/kernel/asm-offsets.c" 1
	
->MM_PGD 36 offsetof(struct mm_struct, pgd)	 #
 # 0 "" 2
 # 183 "arch/mips/kernel/asm-offsets.c" 1
	
->MM_CONTEXT 336 offsetof(struct mm_struct, context)	 #
 # 0 "" 2
 # 184 "arch/mips/kernel/asm-offsets.c" 1
	
->
 # 0 "" 2
 # 185 "arch/mips/kernel/asm-offsets.c" 1
	
->_PGD_T_SIZE 4 sizeof(pgd_t)	 #
 # 0 "" 2
 # 186 "arch/mips/kernel/asm-offsets.c" 1
	
->_PMD_T_SIZE 4 sizeof(pmd_t)	 #
 # 0 "" 2
 # 187 "arch/mips/kernel/asm-offsets.c" 1
	
->_PTE_T_SIZE 4 sizeof(pte_t)	 #
 # 0 "" 2
 # 188 "arch/mips/kernel/asm-offsets.c" 1
	
->
 # 0 "" 2
 # 189 "arch/mips/kernel/asm-offsets.c" 1
	
->_PGD_T_LOG2 2 PGD_T_LOG2	 #
 # 0 "" 2
 # 193 "arch/mips/kernel/asm-offsets.c" 1
	
->_PTE_T_LOG2 2 PTE_T_LOG2	 #
 # 0 "" 2
 # 194 "arch/mips/kernel/asm-offsets.c" 1
	
->
 # 0 "" 2
 # 195 "arch/mips/kernel/asm-offsets.c" 1
	
->_PGD_ORDER 0 PGD_ORDER	 #
 # 0 "" 2
 # 199 "arch/mips/kernel/asm-offsets.c" 1
	
->_PTE_ORDER 0 PTE_ORDER	 #
 # 0 "" 2
 # 200 "arch/mips/kernel/asm-offsets.c" 1
	
->
 # 0 "" 2
 # 201 "arch/mips/kernel/asm-offsets.c" 1
	
->_PMD_SHIFT 22 PMD_SHIFT	 #
 # 0 "" 2
 # 202 "arch/mips/kernel/asm-offsets.c" 1
	
->_PGDIR_SHIFT 22 PGDIR_SHIFT	 #
 # 0 "" 2
 # 203 "arch/mips/kernel/asm-offsets.c" 1
	
->
 # 0 "" 2
 # 204 "arch/mips/kernel/asm-offsets.c" 1
	
->_PTRS_PER_PGD 1024 PTRS_PER_PGD	 #
 # 0 "" 2
 # 205 "arch/mips/kernel/asm-offsets.c" 1
	
->_PTRS_PER_PMD 1 PTRS_PER_PMD	 #
 # 0 "" 2
 # 206 "arch/mips/kernel/asm-offsets.c" 1
	
->_PTRS_PER_PTE 1024 PTRS_PER_PTE	 #
 # 0 "" 2
 # 207 "arch/mips/kernel/asm-offsets.c" 1
	
->
 # 0 "" 2
#NO_APP
	j	$31
	.end	output_mm_defines
	.size	output_mm_defines, .-output_mm_defines
	.section	.text.output_sc_defines,"ax",@progbits
	.align	2
	.globl	output_sc_defines
	.set	nomips16
	.ent	output_sc_defines
	.type	output_sc_defines, @function
output_sc_defines:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
#APP
 # 213 "arch/mips/kernel/asm-offsets.c" 1
	
->#Linux sigcontext offsets.
 # 0 "" 2
 # 214 "arch/mips/kernel/asm-offsets.c" 1
	
->SC_REGS 16 offsetof(struct sigcontext, sc_regs)	 #
 # 0 "" 2
 # 215 "arch/mips/kernel/asm-offsets.c" 1
	
->SC_FPREGS 272 offsetof(struct sigcontext, sc_fpregs)	 #
 # 0 "" 2
 # 216 "arch/mips/kernel/asm-offsets.c" 1
	
->SC_ACX 528 offsetof(struct sigcontext, sc_acx)	 #
 # 0 "" 2
 # 217 "arch/mips/kernel/asm-offsets.c" 1
	
->SC_MDHI 552 offsetof(struct sigcontext, sc_mdhi)	 #
 # 0 "" 2
 # 218 "arch/mips/kernel/asm-offsets.c" 1
	
->SC_MDLO 560 offsetof(struct sigcontext, sc_mdlo)	 #
 # 0 "" 2
 # 219 "arch/mips/kernel/asm-offsets.c" 1
	
->SC_PC 8 offsetof(struct sigcontext, sc_pc)	 #
 # 0 "" 2
 # 220 "arch/mips/kernel/asm-offsets.c" 1
	
->SC_FPC_CSR 532 offsetof(struct sigcontext, sc_fpc_csr)	 #
 # 0 "" 2
 # 221 "arch/mips/kernel/asm-offsets.c" 1
	
->SC_FPC_EIR 536 offsetof(struct sigcontext, sc_fpc_eir)	 #
 # 0 "" 2
 # 222 "arch/mips/kernel/asm-offsets.c" 1
	
->SC_HI1 568 offsetof(struct sigcontext, sc_hi1)	 #
 # 0 "" 2
 # 223 "arch/mips/kernel/asm-offsets.c" 1
	
->SC_LO1 572 offsetof(struct sigcontext, sc_lo1)	 #
 # 0 "" 2
 # 224 "arch/mips/kernel/asm-offsets.c" 1
	
->SC_HI2 576 offsetof(struct sigcontext, sc_hi2)	 #
 # 0 "" 2
 # 225 "arch/mips/kernel/asm-offsets.c" 1
	
->SC_LO2 580 offsetof(struct sigcontext, sc_lo2)	 #
 # 0 "" 2
 # 226 "arch/mips/kernel/asm-offsets.c" 1
	
->SC_HI3 584 offsetof(struct sigcontext, sc_hi3)	 #
 # 0 "" 2
 # 227 "arch/mips/kernel/asm-offsets.c" 1
	
->SC_LO3 588 offsetof(struct sigcontext, sc_lo3)	 #
 # 0 "" 2
 # 228 "arch/mips/kernel/asm-offsets.c" 1
	
->
 # 0 "" 2
#NO_APP
	j	$31
	.end	output_sc_defines
	.size	output_sc_defines, .-output_sc_defines
	.section	.text.output_signal_defined,"ax",@progbits
	.align	2
	.globl	output_signal_defined
	.set	nomips16
	.ent	output_signal_defined
	.type	output_signal_defined, @function
output_signal_defined:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
#APP
 # 259 "arch/mips/kernel/asm-offsets.c" 1
	
->#Linux signal numbers.
 # 0 "" 2
 # 260 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGHUP 1 SIGHUP	 #
 # 0 "" 2
 # 261 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGINT 2 SIGINT	 #
 # 0 "" 2
 # 262 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGQUIT 3 SIGQUIT	 #
 # 0 "" 2
 # 263 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGILL 4 SIGILL	 #
 # 0 "" 2
 # 264 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGTRAP 5 SIGTRAP	 #
 # 0 "" 2
 # 265 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGIOT 6 SIGIOT	 #
 # 0 "" 2
 # 266 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGABRT 6 SIGABRT	 #
 # 0 "" 2
 # 267 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGEMT 7 SIGEMT	 #
 # 0 "" 2
 # 268 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGFPE 8 SIGFPE	 #
 # 0 "" 2
 # 269 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGKILL 9 SIGKILL	 #
 # 0 "" 2
 # 270 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGBUS 10 SIGBUS	 #
 # 0 "" 2
 # 271 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGSEGV 11 SIGSEGV	 #
 # 0 "" 2
 # 272 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGSYS 12 SIGSYS	 #
 # 0 "" 2
 # 273 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGPIPE 13 SIGPIPE	 #
 # 0 "" 2
 # 274 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGALRM 14 SIGALRM	 #
 # 0 "" 2
 # 275 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGTERM 15 SIGTERM	 #
 # 0 "" 2
 # 276 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGUSR1 16 SIGUSR1	 #
 # 0 "" 2
 # 277 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGUSR2 17 SIGUSR2	 #
 # 0 "" 2
 # 278 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGCHLD 18 SIGCHLD	 #
 # 0 "" 2
 # 279 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGPWR 19 SIGPWR	 #
 # 0 "" 2
 # 280 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGWINCH 20 SIGWINCH	 #
 # 0 "" 2
 # 281 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGURG 21 SIGURG	 #
 # 0 "" 2
 # 282 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGIO 22 SIGIO	 #
 # 0 "" 2
 # 283 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGSTOP 23 SIGSTOP	 #
 # 0 "" 2
 # 284 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGTSTP 24 SIGTSTP	 #
 # 0 "" 2
 # 285 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGCONT 25 SIGCONT	 #
 # 0 "" 2
 # 286 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGTTIN 26 SIGTTIN	 #
 # 0 "" 2
 # 287 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGTTOU 27 SIGTTOU	 #
 # 0 "" 2
 # 288 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGVTALRM 28 SIGVTALRM	 #
 # 0 "" 2
 # 289 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGPROF 29 SIGPROF	 #
 # 0 "" 2
 # 290 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGXCPU 30 SIGXCPU	 #
 # 0 "" 2
 # 291 "arch/mips/kernel/asm-offsets.c" 1
	
->_SIGXFSZ 31 SIGXFSZ	 #
 # 0 "" 2
 # 292 "arch/mips/kernel/asm-offsets.c" 1
	
->
 # 0 "" 2
#NO_APP
	j	$31
	.end	output_signal_defined
	.size	output_signal_defined, .-output_signal_defined
	.section	.text.output_irq_cpustat_t_defines,"ax",@progbits
	.align	2
	.globl	output_irq_cpustat_t_defines
	.set	nomips16
	.ent	output_irq_cpustat_t_defines
	.type	output_irq_cpustat_t_defines, @function
output_irq_cpustat_t_defines:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
#APP
 # 297 "arch/mips/kernel/asm-offsets.c" 1
	
->#Linux irq_cpustat_t offsets.
 # 0 "" 2
 # 298 "arch/mips/kernel/asm-offsets.c" 1
	
->IC_SOFTIRQ_PENDING 0 offsetof(irq_cpustat_t, __softirq_pending)	 #
 # 0 "" 2
 # 300 "arch/mips/kernel/asm-offsets.c" 1
	
->IC_IRQ_CPUSTAT_T 32 sizeof(irq_cpustat_t)	 #
 # 0 "" 2
 # 301 "arch/mips/kernel/asm-offsets.c" 1
	
->
 # 0 "" 2
#NO_APP
	j	$31
	.end	output_irq_cpustat_t_defines
	.size	output_irq_cpustat_t_defines, .-output_irq_cpustat_t_defines
	.ident	"GCC: (Buildroot 2012.11.1) 4.6.3"
