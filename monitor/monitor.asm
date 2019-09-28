; Debug monitor code for PSn00bDEBUG by Lameguy64
; 2019 Meido-Tek Productions / PSn00bSDK Project

.psx

.include "cop0regs.inc"
.include "serialregs.inc"

; Vector addresses
BRK_vector	equ		0xA0000040
INT_vector	equ		0xA0000080

; Register array offsets
R_at			equ	0
R_v0			equ	4
R_v1			equ	8
R_a0			equ	12
R_a1			equ	16
R_a2			equ	20
R_a3			equ	24
R_t0			equ	28
R_t1			equ	32
R_t2			equ	36
R_t3			equ	40
R_t4			equ	44
R_t5			equ	48
R_t6			equ	52
R_t7			equ	56
R_s0			equ	60
R_s1			equ	64
R_s2			equ	68
R_s3			equ	72
R_s4			equ	76
R_s5			equ	80
R_s6			equ	84
R_s7			equ	88
R_t8			equ	92
R_t9			equ	96
R_k0			equ	100
R_k1			equ	104
R_gp			equ	108
R_sp			equ	112
R_fp			equ	116
R_ra			equ	120
R_lo			equ	124
R_hi			equ	128
R_epc			equ	132
R_cause			equ	136
R_status		equ	140
R_baddr			equ	144
R_tar			equ	148
R_dcic			equ	152
R_len			equ	156


ERR_ok			equ	0xE0	; Ok/acknowledged response
ERR_invalid		equ	0xE1
ERR_nocont		equ	0xE2	; Cannot continue execution


CMD_ping		equ	0xD0
CMD_info		equ	0xD1
CMD_pause		equ	0xD2
CMD_resume		equ	0xD3
CMD_step		equ	0xD4
CMD_getregs		equ	0xD5
CMD_getmem		equ	0xD6
CMD_getword		equ	0xD7
CMD_setreg		equ	0xD8
CMD_setword		equ	0xD9
CMD_databreak	equ	0xDA
CMD_runto		equ	0xDB
CMD_linestep	equ	0xDC
CMD_reboot		equ	0xDF


EV_start		equ	0xC0
EV_pause		equ	0xC1
EV_except		equ	0xC2
EV_dbgfail		equ	0xC3
EV_break		equ	0xC4


ST_none			equ	0
ST_serial		equ	1
ST_breakpt		equ	2
ST_step			equ	3


; Workaround for missing rfe opcode
.macro rfe
	.word 0x42000010
.endmacro


; Main monitor binary
.create "monitor.bin", 0xC000

; Exception check hook

	nop							; To be patched with a 'standby' jump, applied by
	nop							; patch installer
	nop
	nop
	nop
	
	la		k0, regs
	sw		at, R_at(k0)
	sw		v0, R_v0(k0)
	
	li		v0, SIO_STAT_REG	; Check if there's something in the serial FIFO
	lw		v0, 0(v0)
	mfc0	at, CAUSE
	andi	v0, 0x2
	bnez	v0, serial_int		; Go to handler
	nop
	
	srl		at, 2
	andi	at, 0x1F
	
	li		v0, 0x8				; Let syscalls pass
	beq		at, v0, return
	nop
	
	bnez	at, monitor_entry	; A cause value other than 0 (INT) is a crash
	nop

return:							; Area to be filled by init routine

	la		at, first_int
	lbu		v0, 0(at)
	nop
	bnez	v0, first
	nop

	lw		at, R_at(k0)
	lw		v0, R_v0(k0)
	
	nop
	nop

	db		"KRET"				; To be patched by installer
	nop
	nop
	nop

first:							; Send run event on first interrupt

	sb		r0, 0(at)			
	sw		a0, R_a0(k0)
	sw		ra, R_ra(k0)

	jal		sioSendByte
	li		a0, EV_start
	
	lw		a0, R_a0(k0)
	lw		ra, R_ra(k0)

	b		return
	nop

; Break vector handler

	db		"BRKV"				; Marker for patch installer
	
break_vector:

	la		k0, regs
	sw		at, R_at(k0)
	sw		v0, R_v0(k0)

	; Check if data breakpoint occurred

	lw		v0, 0x3C(r0)			; DCIC
	nop
	andi	at, v0, 0x4				; Data access break bit
	beqz	at, @@no_databreak
	nop

	mfc0	v0, EPC					; Store data access break address
	la		at, data_break_pc
	sw		v0, 0(at)
	la		at, first_int			; Clear first interrupt flag (not needed)
	b		@@no_entrypoint
	sb		r0, 0(at)

@@no_databreak:

	; Data step break

	la		at, data_step
	lbu		v0, 0(at)
	nop
	beqz	v0, @@no_datastep
	nop
	sb		r0, 0(at)

	la		at, data_break			; Check if data break is enabled
	lbu		at, 0(at)
	nop

	andi	at, 0x3					; Enable data access break
	sll		at, 26
	lui		v0, 0xA280
	or		v0, at
	mtc0	v0, DCIC

	la		at, runto_pc			; Check if runto address is set
	lw		v0, 0(at)
	nop
	beqz	v0, @@exit_datastep
	nop
	sw		r0, 0(at)
	mfc0	at, DCIC				; Set run-to as breakpoint
	mtc0	v0, BPC
	lui		v0, 0xA180				; Enable pc break
	or		at, v0
	mtc0	at, DCIC

@@exit_datastep:

	lw		v0, R_v0(k0)			; Resume execution
	lw		at, R_at(k0)
	mfc0	k0, EPC
	nop
	jr		k0
	rfe

@@no_datastep:

	; Break in branch delay handling

	la		at, status				; Skip if not in step state
	lbu		v0, 0(at)
	nop
	bne		v0, ST_step, @@no_step_break
	nop

	lw		v0, 0x3C(r0)			; DCIC
	nop
	andi	at, v0, 0x2				; Check if program counter break
	beqz	at, @@no_step_break
	nop
	mfc0	v0, CAUSE				; Check if break was in a branch delay
	nop
	srl		v0, 30
	andi	at, v0, 0x2				; Check BD
	beqz	at, @@no_step_break
	andi	at, v0, 0x1				; Check BT
	bnez	at, @@branch_taken
	nop

	; If branch not taken

	lui		v0, 0xA180				; Enable BPC break
	mtc0	v0, DCIC
	lw		at, R_at(k0)			; When branch not taken, resume at EPC+8
	lw		v0, R_v0(k0)
	mfc0	k0, EPC
	nop
	addiu	k0,	8					; Set breakpoint after branch
	mtc0	k0, BPC
	nop
	mfc0	k0, EPC					; Resume
	nop
	jr		k0
	rfe

@@branch_taken:

	lui		v0, 0xA180				; Enable BPC break
	mtc0	v0, DCIC
	lw		at, R_at(k0)			; Resume to branch target
	lw		v0, R_v0(k0)
	mfc0	k0, TAR					; Get target address (set in branch delay)
	nop
	mtc0	k0,	BPC					; Set breakpoint to branch target
	nop
	mfc0	k0,	EPC					; Resume
	nop
	jr		k0
	rfe

@@no_step_break:

	la		at, first_int
	lbu		v0, 0(at)
	nop
	beqz	v0, @@no_entrypoint
	nop

	sb		r0, 0(at)			; Send start byte
	sw		a0, R_a0(k0)
	sw		ra, R_ra(k0)

	jal		sioSendByte
	li		a0, EV_start

	lw		a0, R_a0(k0)
	lw		ra, R_ra(k0)

@@no_entrypoint:

	li		v0, ST_breakpt		; Set breakpoint status;
	la		at, status
	sb		v0, 0(at)

	b		monitor_entry_break
	mtc0	r0, DCIC

; Debugger monitor parser begins here
	
serial_int:

	li		v0, ST_serial
	la		at, status
	sb		v0, 0(at)

monitor_entry:

	mfc0	v0, DCIC
	nop
	sw		v0, 0x3C(r0)
	mtc0	r0, DCIC

monitor_entry_break:

	la		at, dbstate
	lbu		v0, 0(at)
	nop
	addiu	v0, 1
	sb		v0, 0(at)

	la		k0, regs
	sw		v1, R_v1(k0)
	sw		a0, R_a0(k0)
	sw		a1, R_a1(k0)
	sw		a2, R_a2(k0)
	sw		a3, R_a3(k0)
	sw		t0, R_t0(k0)
	sw		t1, R_t1(k0)
	sw		t2, R_t2(k0)
	sw		t3, R_t3(k0)
	sw		t4, R_t4(k0)
	sw		t5, R_t5(k0)
	sw		t6, R_t6(k0)
	sw		t7, R_t7(k0)
	sw		s0, R_s0(k0)
	sw		s1, R_s1(k0)
	sw		s2, R_s2(k0)
	sw		s3, R_s3(k0)
	sw		s4, R_s4(k0)
	sw		s5, R_s5(k0)
	sw		s6, R_s6(k0)
	sw		s7, R_s7(k0)
	sw		t8, R_t8(k0)
	sw		t9, R_t9(k0)
	sw		k1, R_k1(k0)
	sw		gp, R_gp(k0)
	sw		sp, R_sp(k0)
	sw		fp, R_fp(k0)
	sw		ra, R_ra(k0)
	mfhi	v0
	mflo	v1
	sw		v0, R_hi(k0)
	sw		v1, R_lo(k0)
	mfc0	a0, EPC
	mfc0	a1, CAUSE
	mfc0	a2, SR
	mfc0	a3, BADVADDR	; BADVADDR
	mfc0	v0, TAR			; TAR
	lw		v1, 0x3C(r0)	; DCIC
	sw		a0, R_epc(k0)
	sw		a1, R_cause(k0)
	sw		a2, R_status(k0)
	sw		a3, R_baddr(k0)
	sw		v0, R_tar(k0)
	sw		v1, R_dcic(k0)

	la		at, status					; Skip exception events if serial IRQ
	lbu		v0, 0(at)
	nop
	beq		v0, ST_serial, parser
	nop
	
	la		at, dbstate					; Check if an exception occurred within
	lbu		v0, 0(at)					; the debug monitor itself
	nop
	bge		v0, 2, debug_fail
	nop

	andi	v1, 0x1						; Check if cop0 break exception
	bnez	v1, break_except
	nop

	srl		a1, 2						; A cause value other than 0 (INT) is a crash
	andi	a1, 0x1F
	beqz	a1, parser
	nop
	
	jal		sioSendByte					; Send exception event
	li		a0, EV_except
	b		parser
	nop

debug_fail:
	
	jal		sioSendByte					; Send debugger failure event
	li		a0, EV_dbgfail
	b		parser
	nop

break_except:

	jal		sioSendByte					; Send cop0 break event
	li		a0, EV_break

	; Debugger command stuff can reside here
	
parser:

	jal		sioReadByte					; Receive a command
	nop
	
	beq		v0, CMD_ping		, ping
	nop
	beq		v0, CMD_info		, info
	nop
	beq		v0, CMD_getregs		, getregs
	nop
	beq		v0, CMD_getmem		, getmem
	nop
	beq		v0, CMD_step		, step
	nop
	beq		v0, CMD_getword		, getword
	nop
	beq		v0, CMD_setreg		, setreg
	nop
	beq		v0, CMD_setword		, setword
	nop
	beq		v0, CMD_databreak	, databreak
	nop
	beq		v0, CMD_runto		, runto
	nop
	
	beq		v0, CMD_reboot		, reboot
	nop	

	beq		v0, CMD_resume		, exec_continue
	nop
	beq		v0, CMD_pause		, exec_pause
	nop
	
command_ret:

	la		v0, status
	lbu		v0, 0(v0)
	nop
	beq		v0, ST_serial, serial_continue
	nop
	
	b		parser
	nop
	

ping:								; -- Ping command --

	jal		sioSendByte
	li		a0, ERR_ok
	b		command_ret
	nop
	
	
info:								; -- Info command --

	jal		sioSendByte
	li		a0, ERR_ok
	la		a1, info_text
@@info_loop:
	lbu		a0, 0(a1)
	addiu	a1, 1
	beqz	a0, @@info_end
	nop
	jal		sioSendByte
	nop
	b		@@info_loop
	nop
@@info_end:
	b		command_ret
	nop

	
getregs:							; -- Registers command --

	jal		sioSendByte
	li		a0, ERR_ok
	la		a1, regs
	li		a2, (R_len/4)-1
@@regs_loop:						; Send all opcodes
	lw		a0, 0(a1)
	addiu	a1, 4
	jal		sioSendReg
	nop
	bgtz	a2, @@regs_loop
	subi	a2, 1
	
	la		a1, regs
	lw		a1, R_epc(a1)			; Get current opcode
	nop
	lw		a0, 0(a1)
	jal		sioSendReg				; Send it off
	nop
	
	b		command_ret
	nop
	
	
getmem:

	jal		sioReadReg			; Get address
	nop
	jal		sioReadReg			; Get length
	move	a1, v0
	subiu	a2, v0, 1
	jal		sioSendByte			; Send OK response
	li		a0, ERR_ok
@@mem_loop:
	lbu		a0, 0(a1)
	jal		sioSendByte
	addiu	a1, 1
	bgtz	a2, @@mem_loop
	addiu	a2, -1
	b		command_ret
	nop

	
step:

	la		v0, dbstate			; Can't continue when monitor exception
	lbu		v0, 0(v0)
	nop
	blt		v0, 2, @@can_step
	nop
	jal		sioSendByte
	li		a0, ERR_nocont
	b		command_ret
	nop
@@can_step:
	lw		v0, R_epc(k0)
	lui		v1, 0xFFFF
	ori		v1, 0xFFFF
	addiu	v0, 4
	mtc0	v0, BPC				; Set PC break and mask
	mtc0	v1, BPCM
	lui		v0, 0xA180
	mtc0	v0, DCIC
	la		v1, status
	li		v0, ST_step
	jal		sioSendByte			; Send OK response
	li		a0, ERR_ok
	b		step_continue
	sb		v0, 0(v1)


runto:

	la		v0, dbstate			; Can't continue when monitor exception
	lbu		v0, 0(v0)
	nop
	blt		v0, 2, @@can_step
	nop
	jal		sioSendByte
	li		a0, ERR_nocont
	b		command_ret
	nop
@@can_step:
	;la		v0, status
	;li		v1, ST_serial
	;sb		v1, 0(v0)
	jal		sioReadReg				; Receive register
	nop
	mtc0	v0, BPC
	lui		v0, 0xA180				; Enable breakpoint
	mtc0	v0, DCIC
	b		exec_continue
	nop


getword:

	jal		sioReadByte				; Get word type
	nop	
	jal		sioReadReg				; Get read address
	move	v1, v0	
	beq		v1, 1, @@word_16
	move	a0, v0
	beq		v1, 2, @@word_32
	nop
	lb		a1, 0(a0)				; Load 8-bit word
	b		@@end
	nop
@@word_16:							; Load 16-bit word
	lh		a1, 0(a0)
	b		@@end
	nop
@@word_32:							; Load 32-bit word
	lw		a1, 0(a0)
	b		@@end
	nop
@@end:
	jal		sioSendByte				; Send OK response
	li		a0, ERR_ok
	jal		sioSendReg				; Send the value
	move	a0, a1
	b		command_ret
	nop


setreg:

	jal		sioReadByte				; Get register number to modify
	nop
	jal		sioReadReg				; Get register value to set
	move	a0, v0
	move	a1, v0
	la		v0, regs				; Get address to register array
	sll		v1, a0, 2				; Multiply register number by 4
	addu	v0, v1					; Add value to register array
	sw		a1, 0(v0)				; Write specified value
	jal		sioSendByte
	li		a0, ERR_ok
	b		command_ret
	nop


setword:

	jal		sioReadByte				; Get word type
	nop
	jal		sioReadReg				; Get write address
	move	v1, v0
	jal		sioReadReg				; Get value to set
	move	a0, v0
	beq		v1, 1, @@word_16
	move	a1, v0
	beq		v1, 2, @@word_32
	nop
	sb		a1, 0(a0)				; Load 8-bit word
	b		@@end
	nop
@@word_16:							; Load 16-bit word
	sh		a1, 0(a0)
	b		@@end
	nop
@@word_32:							; Load 32-bit word
	sw		a1, 0(a0)
	b		@@end
	nop
@@end:
	jal		sioSendReg				; Send the value
	li		a0, ERR_ok
	b		command_ret
	nop


getgte:
	jal		sioReadByte
	nop
	jal		sioSendByte
	li		a0, ERR_ok

	bnez	v0, @@get_control
	nop

	; Collect 32 GTE data registers

	addiu	sp, -128				; Allocate stack memory
	swc2	$0, 0(sp)
	swc2	$1, 4(sp)
	swc2	$2, 8(sp)
	swc2	$3, 12(sp)
	swc2	$4, 16(sp)
	swc2	$5, 20(sp)
	swc2	$6, 24(sp)
	swc2	$7, 28(sp)
	swc2	$8, 32(sp)
	swc2	$9, 36(sp)
	swc2	$10, 40(sp)
	swc2	$11, 44(sp)
	swc2	$12, 48(sp)
	swc2	$13, 52(sp)
	swc2	$14, 56(sp)
	swc2	$15, 60(sp)
	swc2	$16, 64(sp)
	swc2	$17, 68(sp)
	swc2	$18, 72(sp)
	swc2	$19, 76(sp)
	swc2	$20, 80(sp)
	swc2	$21, 84(sp)
	swc2	$22, 88(sp)
	sw		r0 , 92(sp)
	swc2	$24, 96(sp)
	swc2	$25, 100(sp)
	swc2	$26, 104(sp)
	swc2	$27, 108(sp)
	swc2	$28, 112(sp)
	swc2	$29, 116(sp)
	swc2	$30, 120(sp)
	swc2	$31, 124(sp)
	move	a1, sp
	li		a2, 31
@@send_dataregs:
	lw		a0, 0(a1)
	jal		sioSendReg
	addiu	a1, 4
	bgtz	a2, @@send_dataregs
	addi	a2, -1

	b		command_ret
	addiu	sp, 128					; Free stack memory
	
@@get_control:
	jal		sioSendReg
	cfc2	a0, $0
	jal		sioSendReg
	cfc2	a0, $1
	jal		sioSendReg
	cfc2	a0, $2
	jal		sioSendReg
	cfc2	a0, $3
	jal		sioSendReg
	cfc2	a0, $4
	jal		sioSendReg
	cfc2	a0, $5
	jal		sioSendReg
	cfc2	a0, $6
	jal		sioSendReg
	cfc2	a0, $7
	jal		sioSendReg
	cfc2	a0, $8
	jal		sioSendReg
	cfc2	a0, $9
	jal		sioSendReg
	cfc2	a0, $10
	jal		sioSendReg
	cfc2	a0, $11
	jal		sioSendReg
	cfc2	a0, $12
	jal		sioSendReg
	cfc2	a0, $13
	jal		sioSendReg
	cfc2	a0, $14
	jal		sioSendReg
	cfc2	a0, $15
	jal		sioSendReg
	cfc2	a0, $16
	jal		sioSendReg
	cfc2	a0, $17
	jal		sioSendReg
	cfc2	a0, $18
	jal		sioSendReg
	cfc2	a0, $19
	jal		sioSendReg
	cfc2	a0, $20
	jal		sioSendReg
	cfc2	a0, $21
	jal		sioSendReg
	cfc2	a0, $22
	jal		sioSendReg
	cfc2	a0, $23
	jal		sioSendReg
	cfc2	a0, $24
	jal		sioSendReg
	cfc2	a0, $25
	jal		sioSendReg
	cfc2	a0, $26
	jal		sioSendReg
	cfc2	a0, $27
	jal		sioSendReg
	cfc2	a0, $28
	jal		sioSendReg
	cfc2	a0, $29
	jal		sioSendReg
	cfc2	a0, $30
	jal		sioSendReg
	cfc2	a0, $31

	b		command_ret
	nop
	

databreak:							; -- Set data access breakpoint --

	jal		sioReadByte				; Get break flags
	nop
	la		v1, data_break
	sb		v0, 0(v1)
	jal		sioReadReg				; Get break address
	nop
	mtc0	v0, BDA
	jal		sioReadReg				; Get break mask
	nop
	mtc0	v0, BDAM
	jal		sioSendByte				; Send okay response
	li		a0, ERR_ok	
	b		command_ret
	nop

	
reboot:						; -- Soft reboot --

	jal		sioSendByte
	li		a0, ERR_ok

	li		v1, 0x1F801814			; Blank video output
	li		v0, 0x03000001
	sw		v0, 0(v1)
	la		v1, SIO_CTRL_REG		; Reset serial before reboot otherwise
	li		v0, 0x40				; serial will strangely stop listening
	sh		v0, 0(v1)
	lui		v0, 0xBFC0
	jr		v0
	nop

	
exec_pause:							; -- Pause execution --

	la		v0, status				; Clear status
	sb		r0, 0(v0)
	jal		sioSendByte
	li		a0, EV_pause
	b		command_ret
	nop
	

exec_continue:						; -- Continue execution --
	
	la		v1, dbstate				; Check debugger state if resumable
	lbu		v0, 0(v1)
	nop
	blt		v0, 2, @@can_exec		; If less than two, continue
	nop
	jal		sioSendByte				; Send can't continue error
	li		a0, ERR_nocont
	b		command_ret
	nop

@@can_exec:

	jal		sioSendByte				; Send ok response
	li		a0, ERR_ok

serial_continue:					; -- Serial continue --

	mfc0	v0, EPC					; Don't activate data breakpoint if EPC				
	la		v1, data_break_pc		; is equal to data EPC
	lw		v1, 0(v1)
	nop
	beq		v0, v1, @@data_step		; Instead, set a program breakpoint at the
	nop								; next opcode, to set the data breakpoint

	la		v0, runto_pc			; Clear runto command value (only used by
	sw		r0, 0(v0)				; data step)

	la		v0, data_break
	lbu		v0, 0(v0)
	mfc0	a0, DCIC				; Merge data access break enable bits
	beqz	v0, step_continue
	andi	v0, 0x3					; Mask in only first two bits just in case
	sll		v0, 26					; Move it to DR/DW bits of DCIC
	lui		v1, 0xA280				; DCIC base value for data break
	or		v0, v1
	or		v0, a0					; Merge it with current DCIC
	b		step_continue
	mtc0	v0, DCIC

@@data_step:

	la		v1, data_step			; Set data step flag
	li		v0, 1
	sb		v0, 0(v1)

	mfc0	v0, DCIC				; Check if was no run-to command issued
	nop
	srl		v0, 16
	beqz	v0, @@no_runto
	nop

	mfc0	v0, BPC					; Set BPC to run-to for data step
	la		v1, runto_pc
	sw		v0, 0(v1)

@@no_runto:

	lui		v0, 0xA180				; Enable program breakpoint
	mtc0	v0, DCIC
	mfc0	v0, CAUSE
	nop
	srl		v0, 30
	andi	at, v0, 0x2				; Check branch-delay status
	beqz	at, @@no_branch
	andi	at, v0, 0x1				; Check branch-target status
	bnez	at, @@branch_taken
	nop

	; Branch not taken

	mfc0	v0, EPC					; Break past branch (two opcodes after EPC)
	nop
	addiu	v0, 8
	b		step_continue
	mtc0	v0, BPC

@@branch_taken:						; Break on target

	mfc0	v0, TAR
	nop
	b		step_continue
	mtc0	v0, BPC

@@no_branch:

	mfc0	v0, EPC					; Break on next opcode
	nop
	addiu	v0, 4
	mtc0	v0, BPC
	
step_continue:						; -- Step continue --

	la		v0, first_int			; Clear first status
	sb		r0, 0(v0)					
	
	mfc0	v1, EPC					; Skip cop2 opcodes to prevent double exec
	nop
	lw		v0, 0(v1)
	lui		v1, 0xFE00
	and		v0, V1
	lui		v1, 0x4A00
	bne		v0, v1, @@no_cop2_skip
	nop
	
	addiu	v1, 4					; Skip if on cop2
	sw		v1, R_epc(k0)
	
@@no_cop2_skip:

	;jal		FlushCache
	;nop

	la		v0, data_break_pc
	sw		r0, 0(v0)

	la		v1, dbstate
	lbu		v0, 0(v1)
	nop
	subiu	v0, 1
	sb		v0, 0(v1)

	la		k0, regs
	lw		v1, R_v1(k0)			; Restore the registers
	lw		a0, R_a0(k0)
	lw		a1, R_a1(k0)
	lw		a2, R_a2(k0)
	lw		a3, R_a3(k0)
	lw		t0, R_t0(k0)
	lw		t1, R_t1(k0)
	lw		t2, R_t2(k0)
	lw		t3, R_t3(k0)
	lw		t4, R_t4(k0)
	lw		t5, R_t5(k0)
	lw		t6, R_t6(k0)
	lw		t7, R_t7(k0)
	lw		s0, R_s0(k0)
	lw		s1, R_s1(k0)
	lw		s2, R_s2(k0)
	lw		s3, R_s3(k0)
	lw		s4, R_s4(k0)
	lw		s5, R_s5(k0)
	lw		s6, R_s6(k0)
	lw		s7, R_s7(k0)
	lw		t8, R_t8(k0)
	lw		t9, R_t9(k0)
	lw		k1, R_k1(k0)
	lw		gp, R_gp(k0)
	lw		sp, R_sp(k0)
	lw		fp, R_fp(k0)
	lw		ra, R_ra(k0)
	lw		at, R_hi(k0)
	lw		v0, R_lo(k0)
	mthi	at
	mtlo	v0
	
	mfc0	at, CAUSE				; Return to kernel if no exception
	nop
	srl		at, 2
	andi	at, 0x1F
	li		v0, 0x8
	beq		at, v0, return
	nop
	beqz	at, return
	nop
	
	lw		at, R_at(k0)
	lw		v0, R_v0(k0)
	nop
	lw		k0, R_epc(k0)
	nop
	jr		k0
	rfe

;FlushCache:
;	addiu	t2, r0, 0xA0
;	jr		t2
;	addiu	t1, r0, 0x44

	
.include "serial.inc"
	
; Variables/data area

first_int:		.byte 1
status:			.byte 0
dbstate:		.byte 0
data_break:		.byte 0
data_step:		.byte 0
				.byte 0
				.byte 0
				.byte 0
data_break_pc:	.word 0
runto_pc:		.word 0

regs:			.fill	R_len, 0

info_text:		.asciiz "PSn00bDEBUG Monitor v0.2 by Lameguy64"

.close