.segment "CODE"

nmi_handler:
	; OAM DMA update
	pha
	php
	
	lda #$00
	sta $2003
	lda #$02
	sta $4014
	
	; inc cycle_flag
	
	plp
	pla
irq_handler:
	rti
	
.export nmi_handler
.export irq_handler