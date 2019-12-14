MAX_VEL_X = 128
MAX_VEL_Y = 128

SPRITE = $200

	.zp
; Player data
ppos_x: ds 2
ppos_y: ds 2
pvel_x: ds 1
pvel_y: ds 1
pacc_x: ds 1
pacc_y: ds 1

	.code
	.bank 0
player_integrate:
; Integrate acceleration x
	clc
	lda pvel_x
	adc pacc_x
; Clamp velocity
	
	lda pvel_x
	; Test sign bit
	and #1 << 7
	beq .x_sign_clear

	cmp #-MAX_VEL_X
	bpl x_finished
	lda #-MAX_VEL_X
	sta pvel_x
	
.x_sign_clear:
	cmp #MAX_VEL_X
	bpl x_finished
	lda MAX_VEL_X
	sta pvel_x
	
	; integrate low byte
	adc ppos_x
	sta ppos_x
	; integrate high byte
	lda #$00
	adc ppos_x + 1
	sta ppos_x + 1
	

x_finished:

	; Y velocity
	clc
	lda pvel_y
	adc pacc_y
	
	lda pvel_y
	; Test sign bit
	and #1 << 7
	beq .y_sign_clear

	cmp #-MAX_VEL_Y
	bpl y_finished
	lda #-MAX_VEL_Y
	sta pvel_y
	
.y_sign_clear:
	cmp #MAX_VEL_Y
	bpl y_finished
	lda MAX_VEL_Y
	sta pvel_y
	
y_finished:

	; integrate low byte
	adc ppos_y
	sta ppos_y
	; integrate high byte
	lda #$00
	adc ppos_y + 1
	sta ppos_y + 1

	rts
	
player_tick:
	ldx #$00

	lda control_1
	and #$01
	beq .done_test_right
	ldx #$01
.done_test_right:
	lda control_1
	and #1
	beq .done_test_left
	ldx #-1
.done_test_left:
	stx pacc_x
	jsr player_integrate
	
; copy to player sprite
	ldx ppos_y + 1
	dex
	stx SPRITE
	ldx ppos_x + 1
	stx SPRITE + 3
	
	rts
	
player_init:
; initialize player sprite
	ldx #$70
	stx SPRITE
	stx SPRITE + 3
	
; also write to high pos byte
	stx ppos_x + 1
	stx ppos_y + 1
	
	ldx #$00
	stx SPRITE + 1
	stx SPRITE + 2
	
; initialize rest of player data
	stx pacc_x
	stx pacc_y
	stx pvel_x
	stx pvel_y
	stx ppos_x
	stx ppos_y
		
	rts