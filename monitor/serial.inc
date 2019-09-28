;; Serial communication routines
;; Does not use interrupts!

sioSendReg:
	; Send 4 bytes of a register to the serial port.
	; Input: a0 - Value to send.
	; Return: none.
	addiu	sp, -24
	sw		v0, 0(sp)
	sw		v1, 4(sp)
	sw		a0, 8(sp)
	sw		a1, 12(sp)
	sw		a2, 16(sp)
	sw		ra, 20(sp)
	move	a1, zero
	move	a2, a0
.write_loop:
	andi	a0, a2, 0xFF
	jal		sioSendByte
	srl		a2, 8
	li		at, 3
	bne		a1, at, .write_loop
	addiu	a1, 1
	lw		v0, 0(sp)			; Restore stack and return
	lw		v1, 4(sp)
	lw		a0, 8(sp)
	lw		a1, 12(sp)
	lw		ra, 20(sp)
	lw		a2, 16(sp)
	jr		ra
	addiu	sp, 24
	
sioReadReg:
	; Receives 4 bytes from the serial port into a register
	; Input: none.
	; Return: v0 - Received word.
	addiu	sp, -16
	sw		ra, 0(sp)
	sw		v1, 4(sp)
	sw		a0, 8(sp)
	move	v1, r0
	move	a0, r0
.loop:
	jal		sioReadByte
	nop
	sllv	v0, a0
	addiu	a0, 8
	slti	at, a0, 32
	bnez	at, .loop
	or		v1, v0
	move	v0, v1
	lw		ra, 0(sp)
	lw		v1, 4(sp)
	lw		a0, 8(sp)
	jr		ra
	addiu	sp, 16
	
sioReadByte:
	; Read a byte from the serial port.
	; Returns: v0 - byte read.
	la		v0, SIO_STAT_REG	; Read value from serial
	lw		v0, 0(v0)
	nop
	andi	v0, 0x2
	beqz	v0, sioReadByte
	nop
	la		v0, SIO_TXRX_REG
	lbu		v0, 0(v0)
	jr		ra
	nop
	
sioSendByte:
	; Send a byte to the serial port.
	; Input: a0 - byte to send to serial.
	; Returns: none
	addiu	sp, -4
	sw		v0, 0(sp)
.tx_ready:						; Wait for TX to become ready
	la		v0, SIO_STAT_REG
	lhu		v0, 0(v0)
	nop	
	andi	v0, 0x1
	beqz	v0, .tx_ready
	nop
	la		v0, SIO_TXRX_REG		; Send byte
	sb		a0, 0(v0)
.tx_done:						; Wait for TX to finish
	la		v0, SIO_STAT_REG
	lhu		v0, 0(v0)
	nop
	andi	v0, 0x4
	beqz	v0, .tx_done
	nop
	lw		v0, 0(sp)			; Restore stack and return
	jr		ra
	addiu	sp, 4