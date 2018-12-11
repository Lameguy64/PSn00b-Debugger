	opt	m+,l.,c+
	
	include 'dbstub.inc'
	
	org $A0010000

BREAK_ADDR		equ		$0040
HOOK_ADDR		equ		$0088

INSTALL_ADDR	equ		$c000
	
; Note: Only execute this routine with callbacks and interrupts turned off.

install:	
	la		a0, install_data	; Copy debug stub program to install address
	li		a1, INSTALL_ADDR
	lui		v0, $a000			; Set address to uncached space
	or		a1, v0
	la		a2, (install_data_end-install_data)>>2
	
.copy_loop
	lw		v0, 0(a0)
	addiu	a0, 4
	sw		v0, 0(a1)
	addiu	a1, 4
	bgtz	a2, .copy_loop
	addi	a2, -1
	
	mfc0	v1,SR				; Get Status Register
	andi	v0,zero,$fffc		; Mask
	and		v0,v0,v1			; Interrupts Off
	mtc0	v0,SR
	nop
	nop
	
	li		a0, HOOK_ADDR		; Patch the hook
	lui		v0, $a000
	or		a0, v0
	li		v0, $341AC000
	sw		v0, 0(a0)
	li		v0, $03400008
	sw		v0, 4(a0)
	sw		r0, 8(a0)
	
	li		a0, BREAK_ADDR		; Patch the cop0 break vector
	lui		v0, $a000			; (for break on program entrypoint)
	or		a0, v0
	li		v0, $40803800
	sw		v0, 0(a0)
	li		v0, $341AC000
	sw		v0, 4(a0)
	li		v0, $03400008
	sw		v0, 8(a0)
	sw		r0, 12(a0)
	
	sw		r0, $7c(r0)
	
	mtc0	zero,BPC			; Set cop0 BPC and BDA to zero
	mtc0	zero,BDA
	li		a0,$ffffffff		; Set cop0 BPC and BDA masks
	mtc0	a0,BDAM
	mtc0	a0,BPCM
	
	lui		v0,$e180
	mtc0	v0,DCIC
	
	jr		ra
	mtc0	v1,SR				; Restore Interrupt State
	
install_data:
	incbin 'dbstub.bin'
install_data_end: