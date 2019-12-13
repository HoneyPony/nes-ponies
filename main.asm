	.include "constants.asm"
	.include "header.asm"

	.bank 0
	.org $C000

	.include "nmi.asm"
	.include "reset.asm"
	
fill_nametable:
	ldx PPUSTATUS
	ldx #$20
	stx PPUADDR
	ldx #$00
	stx PPUADDR
	
	lda #$FF
	ldy #$00
	
	.yloop:
		ldx #$00
		.xloop:
			
			
			sta PPUDATA
			inx
			bne .xloop
		
		iny
		cpy #$4
		bne .yloop
		
	rts
	
draw_ground:
	ldx PPUSTATUS
	ldx #$21
	stx PPUADDR
	ldx #$C7
	stx PPUADDR
	
	lda #$04
	sta PPUDATA
	
	lda #$00
	ldx #$00
	.xloop:
		sta PPUDATA
		inx
		adc #1
		and #%00000011
		cpx #18
		bne .xloop
	
	lda #$05
	sta PPUDATA
		
	rts
	
	
main:
	; Load from ppustatus before writing palettes
	; This ensures that PPUADDR accepts high byte first
	ldx PPUSTATUS
	ldx #$3f
	stx PPUADDR
	ldx #$00
	stx PPUADDR
	
	; Palette 0
	ldx #$00
.write_palette:
	
	lda #$0A
	sta PPUDATA
	lda #$17
	sta PPUDATA
	lda #$07
	sta PPUDATA
	lda #$19
	sta PPUDATA
	inx
	cpx #8
	bne .write_palette
	
	ldx #$FF
	txs
	
	jsr fill_nametable
	jsr draw_ground
	
	; Reset PPUADDR ..?
	ldx PPUSTATUS
	ldx #$20
	stx PPUADDR
	ldx #$00
	stx PPUADDR
	
	lda #%10000000
	sta PPUCTRL
	lda #%00011110
	sta PPUMASK
	
	.forever:
		jmp .forever


	.bank 1
	.org $FFFA
	.dw nmi_handler
	.dw reset_handler
	.dw 0

	.bank 2
	.org $0000
	.incbin "graphics.chr"
