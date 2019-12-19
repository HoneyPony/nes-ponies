.segment "CODE"

nmi_handler:
	; OAM DMA update
	pha
	txa
	pha
	tya
	pha
	php
	
	lda $2002
	lda $00
	sta $2005
	sta $2005 ; Set scroll to zero
	
	; Copy Sprite OAM
	lda #$00
	sta $2003
	lda #$02
	sta $4014
	
	; inc cycle_flag
	
	plp
	pla
	tay
	pla
	tax
	pla
irq_handler:
	rti
	
.export nmi_handler
.export irq_handler