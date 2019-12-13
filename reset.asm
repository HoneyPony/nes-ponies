reset_handler:
	sei
	cld
	lda #$00
	sta PPUCTRL
	sta PPUMASK
.vblankwait:
	bit PPUSTATUS
	bpl .vblankwait
	jmp main