; PSn00b Debugger kernel patch by Lameguy64 of Meido-Tek Productions

; Exception vector notes:
;	When the CPU jumps to either a exception or break vector, the CPU is still in the same segment
;	as the running program (usually $80000000) which means cop0 trace for kernel mode is
;	still effective in the exception vector. Be wary of this when utilizing trace mode when writing
;	your own debugger.

	opt	at-,m+,l.,c+
	
	include 'dbstub.inc'
	include 'serialregs.inc'
	
	org $0000c000

MAX_BREAKS	equ		8			; Number of breakpoint slots (each slot takes 8 bytes)
	
ST_BEGIN	equ		255			; Initial status value
ST_NORMAL	equ		0
ST_SERIAL	equ		1			; Interrupt by serial command
ST_CBREAK	equ		2			; Break by cop0

DB_BEGIN	equ		$4E47427E	; Program begin message
DB_EXCEPT	equ		$4358457E	; Exception message
DB_SUSPEND	equ		$5355537E	; Suspend message
DB_STEP		equ		$5054537E	; Step message
DB_BREAK	equ		$4B52427E	; Breakpoint message

HOOK_ADDR	equ		$A0000088
BREAK_ADDR	equ		$A0000040	; cop0 break interrupt vector address (uncached)


Start:		; Start of debug stub
	li		k0, $c80			; To temporarily disable patch by default to let loaders
	jr		k0					; use the serial (to be patched with nops to engage)
	nop
	
	la		k0, dbgRegs
	sw		at, rat(k0)
	sw		v0, rv0(k0)
	
	lui		at, $1f06			; Heartbeat
	sb		r0, 1(at)
	
	li		v0, SIO_STAT_REG	; Check if theres something in the serial
	lw		v0, 0(v0)
	mfc0	at, CAUSE
	andi	v0, $2
	bnez	v0, SerialInt		; Go to handler
	nop
	
	mfc0	v0, EPC
	srl		at, 2
	andi	at, $1f
	
	li		v0, $8				; Let syscalls pass
	beq		at, v0, Return
	nop
	
	bnez	at, Handler			; Trap everything else but interrupts
	nop
	
Return:		; Return to BIOS exception handler
	lw		v0, rv0(k0)
	lw		at, rat(k0)
	li		k0, $c80
	jr		k0
	nop
	

SerialInt:	; Interrupt by serial command
	mtc0	r0, DCIC
	la		at, dbgStatus
	li		v0, ST_SERIAL
	sb		v0, 0(at)
	b		Handler
	nop

BreakInt:	; Interrupt by cop0 break
	la		k0, dbgRegs
	sw		v0, rv0(k0)
	sw		at, rat(k0)
	
	la		at, dbgStatus
	li		v0, ST_CBREAK
	sb		v0, 0(at)
	
Handler:	; The debugger handler
	sw		r0, rzero(k0)		; Save the registers!
	sw		v1, rv1(k0)
	sw		a0, ra0(k0)
	sw		a1, ra1(k0)
	sw		a2, ra2(k0)
	sw		a3, ra3(k0)
	sw		t0, rt0(k0)
	sw		t1, rt1(k0)
	sw		t2, rt2(k0)
	sw		t3, rt3(k0)
	sw		t4, rt4(k0)
	sw		t5, rt5(k0)
	sw		t6, rt6(k0)
	sw		t7, rt7(k0)
	sw		s0, rs0(k0)
	sw		s1, rs1(k0)
	sw		s2, rs2(k0)
	sw		s3, rs3(k0)
	sw		s4, rs4(k0)
	sw		s5, rs5(k0)
	sw		s6, rs6(k0)
	sw		s7, rs7(k0)
	sw		t8, rt8(k0)
	sw		t9, rt9(k0)
	sw		k0, rk0(k0)
	sw		k1, rk1(k0)
	sw		gp, rgp(k0)
	sw		sp, rsp(k0)
	sw		fp, rfp(k0)
	sw		ra, rra(k0)
	mfhi	v0
	sw		v0, rhi(k0)
	mflo	v0
	sw		v0, rlo(k0)
	mfc0	v0, SR
	mfc0	v1, CAUSE
	mfc0	a0, EPC
	sw		v0, rstatus(k0)
	sw		v1, rcause(k0)
	sw		a0, repc(k0)
	
	la		at, dbgStatus			; Get status
	lbu		v0, 0(at)
	
	li		v1, ST_BEGIN			; Program start break
	beq		v0, v1, DebugBegin
	nop
	li		v1, ST_CBREAK			; Break by cop0
	beq		v0, v1, CopBreak
	nop
	li		v1, ST_SERIAL			; Check if serial command
	beq		v0, v1, ControlLoop
	nop

	la		a0, DB_EXCEPT			; If exception was neither a step or serial,
	jal		sioSendReg				; It must be an actual crash so send appropriate message
	nop
	b		ControlLoop
	nop
	
CopBreak:
	li		a0, DB_BREAK
	jal		sioSendReg
	nop
	b		ControlLoop
	nop
	
DebugBegin:	; Init function
	li		a0, DB_BEGIN
	jal		sioSendReg
	nop
	jal		InstallVectors			; Install new vector hooks
	nop
	la		at, dbgStatus
	b		ControlLoop
	sb		r0, 0(at)
	
	
ControlLoop:	; Debug loop
	jal		sioReadByte				; Receive a command
	nop
	
	li		v1, 'I'					; Init check
	beq		v0, v1, InitCommand
	nop
	li		v1, 'R'					; Output registers
	beq		v0, v1, GetRegisters
	nop
	li		v1, 'D'					; Output memory
	beq		v0, v1, GetMemory
	nop
	
	li		v1, 'C'					; Continue execution
	beq		v0, v1, Continue
	nop
	li		v1, 'P'					; Suspend execution
	beq		v0, v1, SuspendCommand
	nop
	li		v1, 'T'					; Run to address
	beq		v0, v1, RunToAddr
	nop
	
	li		v1, 'Z'
	beq		v0, v1, Reboot
	nop
	
LoopReturn:
	la		at, dbgStatus
	lbu		v0, 0(at)
	li		at, ST_SERIAL
	
	beq		v0, at, Continue
	nop
	
	b		ControlLoop
	nop
	
	
; Init check command (sends a string)
InitCommand:
	la		a1, dbgInfo
	la		a2, (dbgInfoEnd-dbgInfo)-1
.init_loop
	lbu		a0, 0(a1)
	jal		sioSendByte
	addiu	a1, 1
	bgtz	a2, .init_loop
	subi	a2, 1
	b		LoopReturn
	nop
	
	
; Suspend execution
SuspendCommand:
	mtc0	r0, DCIC
	li		a0, DB_SUSPEND		; Send suspend response
	jal		sioSendReg
	nop
	la		at, dbgStatus
	b		LoopReturn
	sb		r0, 0(at)			; Clear status flag


; Get CPU registers + current opcode
GetRegisters:
	la		a1, dbgRegs
	li		a2, 36
.regs_loop
	lw		a0, 0(a1)
	addiu	a1, 4
	jal		sioSendReg
	nop
	bgtz	a2, .regs_loop
	subi	a2, 1
	
	la		a1, dbgRegs
	lw		a1, repc(a1)		; Get current opcode
	jal		sioSendReg			; Send it off
	lw		a0, 0(a1)
	b		LoopReturn
	nop
	
	
; Get data from console memory
GetMemory:
	jal		sioReadReg			; Get address
	nop
	move	a1, v0
	jal		sioReadReg			; Get length
	nop
	subi	a2, v0, 1
.mem_loop
	lbu		a0, 0(a1)
	jal		sioSendByte
	nop
	addiu	a1, 1
	bgtz	a2, .mem_loop
	subi	a2, 1
	b		LoopReturn
	nop

	
; Run until address
RunToAddr:
	la		at, BREAK_ADDR		; Install break vector
	li		v0, $40803800
	sw		v0, 0(at)
	li		v0, $341A0000|(BreakInt&$ffff)
	sw		v0, 4(at)
	li		v0, $03400008
	sw		v0, 8(at)
	sw		r0, 12(at)	
	jal		sioReadReg			; Get break address
	nop
	mtc0	v0, BPC
	lui		v0, $e180
	mtc0	v0, DCIC	
	la		at, dbgStatus		; Set stepping flag
	b		Continue
	sb		r0, 0(at)


; Soft reboot
Reboot:
	li		k0, $1F801814		; Blank video output (to not confuse the user)
	li		v0, $03000001
	sw		v0, 0(k0)
	la		at, SIO_CTRL_REG	; Reset serial before reboot otherwise
	li		v0, $40				; serial will stop listening for some reason.
	sh		v0, 0(at)
	li		k0, $BFC00000		; Jump to BIOS
	jr		k0
	rfe
	
	
; Continue execution
Continue:
	
	lw		v1, repc(k0)
	nop
	lw		v0, 0(v1)
	lui		at, $fe00
	and		v0, at
	lui		at, $4a00
	bne		v0, at, .no_cop2_skip
	nop
	
	addiu	v1, 4
	sw		v1, repc(k0)
	
.no_cop2_skip:

	lw		v1, rv1(k0)			; Restore the registers
	lw		a0, ra0(k0)
	lw		a1, ra1(k0)
	lw		a2, ra2(k0)
	lw		a3, ra3(k0)
	lw		t0, rt0(k0)
	lw		t1, rt1(k0)
	lw		t2, rt2(k0)
	lw		t3, rt3(k0)
	lw		t4, rt4(k0)
	lw		t5, rt5(k0)
	lw		t6, rt6(k0)
	lw		t7, rt7(k0)
	lw		s0, rs0(k0)
	lw		s1, rs1(k0)
	lw		s2, rs2(k0)
	lw		s3, rs3(k0)
	lw		s4, rs4(k0)
	lw		s5, rs5(k0)
	lw		s6, rs6(k0)
	lw		s7, rs7(k0)
	lw		t8, rt8(k0)
	lw		t9, rt9(k0)
	lw		k1, rk1(k0)
	lw		gp, rgp(k0)
	lw		sp, rsp(k0)
	lw		fp, rfp(k0)
	lw		ra, rra(k0)
	lw		at, rhi(k0)
	lw		v0, rlo(k0)
	mthi	at
	mtlo	v0
	lw		at, rat(k0)
	lw		v0, rv0(k0)
	lw		k0, repc(k0)
	nop
	jr		k0
	rfe


; Vector install function
InstallVectors:

	; Install new exception vector
	la		at, HOOK_ADDR
	li		v0, $341AC000		; ori k0, r0, $c000
	sw		v0, 0(at)
	li		v0, $03400008		; jr k0
	sw		v0, 4(at)
	sw		r0, 12(at)
	
	; Install break vector
	la		at, BREAK_ADDR
	li		v0, $40803800		; mtc0 r0, r7
	sw		v0, 0(at)
	li		v0, $341A0000|(BreakInt&$ffff)	; ori k0, r0, BreakInt&ffff
	sw		v0, 4(at)
	li		v0, $03400008		; jr k0
	sw		v0, 8(at)
	jr		ra
	sw		r0, 12(at)
	
	
;; Serial stuff
	include 'serial.inc'

	
;; Stub information
dbgInfo
	db "DBGN00B-PSX V0 BY LAMEGUY64",0
dbgInfoEnd
	
	
;; Debugger variables
dbgStatus		db		ST_BEGIN		; Status/operation number
dbgPad			db		$0
				db		$0
				db		$0

dbgRegs			dsb regsize				; Register storage	
