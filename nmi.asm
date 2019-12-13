nmi_handler:
	; OAM DMA update
	lda #$00
	sta OAMADDR
	lda #$02
	sta OAMDMA
	rti