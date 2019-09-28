	opt	m+,l.,c+
	
	section .text
	
	global psdb_install
	

BPC			equs	"r3"
DCIC		equs	"r7"
BPCM		equs	"r11"
SR			equs	"r12"

SIO_MODE_REG	equ		$1058
SIO_CTRL_REG	equ		$105A
SIO_BAUD_REG	equ		$105E
SIO_BAUD		equ		$12			; 115200 baud

psdb_install:

	mfc0	t1, SR				; Get Status Register
	andi	v0, r0, 0xFFFC		; Mask
	and		v0, v0, t1			; Interrupts Off
	mtc0	v0, SR
	nop
	nop
	
	la		a0, patch_start		; Copy debug stub program to install address
	li		a1, 0xA000C000		; Set address to uncached space
	la		a2, ((patch_end-patch_start)/4)
.copy_loop:
	lw		v0, 0(a0)
	addiu	a0, 4
	sw		v0, 0(a1)
	addiu	a1, 4
	bgtz	a2, .copy_loop
	addi	a2, -1
	
	lw		a0, 0x80(r0)		; Copy old hook
	lw		a1, 0x84(r0)
	lw		a2, 0x88(r0)
	lw		a3, 0x8C(r0)
	
	;la		v0, 0xA000C000
	;sw		a0, 0x0(v0)			; Store to start of patch (for standby)
	;sw		a1, 0x4(v0)
	;sw		a2, 0x8(v0)
	;sw		a3, 0xc(v0)
	
	li		v1, 0x5445524B		; KRET
	li		v0, 0xA000C000
.patch_search:
	lw		t0, 0(v0)
	nop
	bne		t0, v1, .patch_search
	addiu	v0, 4
	addiu	v0, -4
	
	sw		a0, 0x0(v0)			; Store a copy on KRET marker
	sw		a1, 0x4(v0)
	sw		a2, 0x8(v0)
	sw		a3, 0xC(v0)
	
	la		v1, 0xA0000080		; Install the new hook
	la		v0, 0x341AC000
	sw		v0, 8(v1)
	lui		v0, 0x0340
	ori		v0, 0x0008
	sw		v0, 12(v1)
	sw		r0, 16(v1)

	la		a0, 0xA0000040		; Setup break vector hook
	sw		r0, -4(a0)
	la		v1, 0x401A3800		; mfc0 k0, DCIC
	sw		v1, 0(a0)
	sw		r0, 4(a0)			; nop
	la		v1,	0xAC1A003C		; sw k0, 0x3C(r0)
	sw		v1, 8(a0)
	la		v0, 0x40803800		; mtc0 r0, DCIC
	sw		v0, 12(a0)

	li		v1, 0x564B5242		; Find breakpoint marker in monitor
	li		v0, 0xA000C000
.break_search:
	lw		t0, 0(v0)
	nop
	bne		t0, v1, .break_search
	addiu	v0, 4

	andi	v0, 0xFFFF			; Write a jump to that point
	lui		v1, 0x341A
	or		v0, v1
	sw		v0, 16(a0)
	la		v0, 0x03400008
	sw		v0, 20(a0)
	sw		r0, 24(a0)

	lui		v0, 0xFFFF			; Enable PC address break to break on entry
	ori		v0, 0xFFFF
	mtc0	v0, BPCM
	mtc0	r0, BPC
	lui		v0, 0xE180
	mtc0	v0, DCIC

	; Configure serial interface
	lui		a0,	0x1F80
	li		v0, 0x40
	sh		v0, SIO_CTRL_REG(a0)
	li		v0, 0x4E
	sh		v0, SIO_MODE_REG(a0)
	li		v0, SIO_BAUD
	sh		v0, SIO_BAUD_REG(a0)
	li		v0, 0x5
	sh		v0, SIO_CTRL_REG(a0)
	nop
	nop
	jr		ra
	mtc0	t1, SR
	
	section .data
	
patch_start:
	incbin "..\monitor.bin"
patch_end: