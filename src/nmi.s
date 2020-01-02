.export _cycle_flag

.segment "BSS"
_cycle_flag: .res 1

.segment "CODE"

nmi_handler:
	; OAM DMA update
	pha
	tya
	pha
	txa
	pha
	
	lda $2002
	lda #$20
	sta $2006
	lda #$00
	sta $2006
	
	sta $2005
	sta $2005 ; Set scroll to zero
	
	; Copy Sprite OAM
	lda #$00
	sta $2003
	lda #$02
	sta $4014
	
	inc _cycle_flag
	
	pla
	tax
	pla
	tay
	pla
irq_handler:
	rti
	
.export nmi_handler
.export irq_handler