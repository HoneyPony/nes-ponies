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
	
sample_palettes:
	; Load from ppustatus before writing palettes
	; This ensures that PPUADDR accepts high byte first
	ldx PPUSTATUS
	ldx #$3f
	stx PPUADDR
	ldx #$00
	stx PPUADDR
	
	; Palettes
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
	
	; sprite palette
	ldx PPUSTATUS
	ldx #$3f
	stx PPUADDR
	ldx #$10
	stx PPUADDR
	
	lda #$0f
	sta PPUDATA
	lda #$11
	sta PPUDATA
	lda #$21
	sta PPUDATA
	lda #$31
	sta PPUDATA
	
	rts
	
sample_sprite_init:
	; sprite
	lda #$70
	sta $200
	lda #$00
	sta $201
	lda #%00000000
	sta $202
	lda #$70
	sta $203
	
	; position data: $010:$011 = x, $012:$013 = y
	lda #$70
	sta $010
	lda #$00
	sta $011
	
	lda #$70
	sta $012
	lda #$00
	sta $013
	
	rts
	
sample_sprite_update:
	clc
	lda $011
	adc #$01
	sta $011
	lda $010
	adc #$00
	sta $010
	sta $203
	
	rts
	
sample_sprite_update_2:
	clc
	lda $021
	adc #$01
	sta $021
	lda $020
	adc #$00
	sta $020
	sta $020
	
	rts
	
tick:
	jsr sample_sprite_update
	jsr sample_sprite_update_2 ; if we comment this out the sprite moves twice as fast

	jmp tick
	

	
main:
	jsr sample_palettes
	jsr sample_sprite_init
	jsr fill_nametable
	jsr draw_ground
	
	; Reset PPUADDR ..?
	ldx PPUSTATUS
	ldx #$20
	stx PPUADDR
	ldx #$00
	stx PPUADDR
	
	; Enable rendering
	lda #%10001000
	sta PPUCTRL
	lda #%00011110
	sta PPUMASK
	
	jmp tick


	.bank 1
	.org $FFFA
	.dw nmi_handler
	.dw reset_handler
	.dw 0

	.bank 2
	.org $0000
	.incbin "graphics.chr"
