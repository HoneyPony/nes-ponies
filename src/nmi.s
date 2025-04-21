.global _cycle_flag

.section .bss
_cycle_flag: .fill 1

.section .text

nmi_handler:
	; OAM DMA update
	pha
	tya
	pha
	txa
	pha
	
	lda 2002
	lda #20
	sta 2006
	lda #00
	sta 2006
	
	sta 2005
	sta 2005 ; Set scroll to zero
	
	; Copy Sprite OAM
	lda #00
	sta 2003
	lda #02
	sta 4014
	
	inc _cycle_flag
	
	pla
	tax
	pla
	tay
	pla
irq_handler:
	rti
	
.global nmi_handler
.global irq_handler