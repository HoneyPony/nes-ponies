	.include "constants.asm"
	.include "header.asm"

	.zp
	control_1: ds 1
	control_2: ds 1
	player_x: ds 2
	player_y: ds 2
	
	.code
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
	sta player_x
	lda #$00
	sta player_x + 1
	
	lda #$70
	sta player_y
	lda #$00
	sta player_y + 1
	
	rts
	
sample_sprite_mr:
	clc
	lda player_x + 1
	adc #$01
	sta player_x + 1
	lda player_x 
	adc #$00
	sta player_x
	sta $203
	
	rts
	
sample_sprite_ml:
	clc
	lda player_x + 1
	sbc #$01
	sta player_x + 1
	lda player_x 
	sbc #$00
	sta player_x
	sta $203
	
	rts
	
readjoy:
    lda #$01
    ; While the strobe bit is set, buttons will be continuously reloaded.
    ; This means that reading from JOYPAD1 will only return the state of the
    ; first button: button A.
    sta JOYPAD1
    sta control_1
    lsr a        ; now A is 0
    ; By storing 0 into JOYPAD1, the strobe bit is cleared and the reloading stops.
    ; This allows all 8 buttons (newly reloaded) to be read from JOYPAD1.
    sta JOYPAD1
loop:
    lda JOYPAD1
    lsr a	       ; bit 0 -> Carry
    rol control_1  ; Carry -> bit 0; bit 7 -> Carry
    bcc loop
    rts
	
idle:
	jsr readjoy
	lda control_1
	and #$01
	beq .next_test
	jsr sample_sprite_mr
.next_test:
	lda control_1
	and #%00000010
	beq .after_left
	jsr sample_sprite_ml
.after_left
	
	jmp idle
	

	
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
	
	jmp idle


	.bank 1
	.org $FFFA
	.dw nmi_handler
	.dw reset_handler
	.dw 0

	.bank 2
	.org $0000
	.incbin "graphics.chr"
