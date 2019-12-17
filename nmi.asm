.proc nmi_handler
	; OAM DMA update
	pha
	php
	
	lda #$00
	sta OAMADDR
	lda #$02
	sta OAMDMA
	
	inc cycle_flag
	
	plp
	pla
	rti
.endproc