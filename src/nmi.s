.global cycle_flag

.section .bss
cycle_flag: .fill 1

.section .text
nmi:
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
	
	inc cycle_flag
	
	pla
	tax
	pla
	tay
	pla
irq:
	rti
	
.global nmi
.global irq