MAX_VEL_X = 24
MAX_VEL_Y = 24

SPRITE = $200

	.zp
; Player data
ppos_x: ds 2
ppos_y: ds 2
pvel_x: ds 2
pvel_y: ds 1
pacc_x: ds 2
pacc_y: ds 1

	.data
	.bank 0
	.org $C000
	
	.code
	.bank 0
	
player_integrate:
; Integrate acceleration x
	
	; low byte
	clc
	lda pacc_x
	adc pvel_x
	sta pvel_x
	lda pacc_x + 1
	adc pvel_x + 1
	sta pvel_x + 1

; Clamp velocity: only need to read from low byte for now
	clc
	lda pvel_x
	bmi .x_negative

	cmp #MAX_VEL_X
	bmi .x_integrate_pos
	lda #$00
	sta pvel_x + 1
	lda #MAX_VEL_X
	sta pvel_x
	
.x_negative:
	cmp #-MAX_VEL_X
	bpl .x_integrate_pos
	lda #$FF
	sta pvel_x + 1
	lda #-MAX_VEL_X
	sta pvel_x
	
.x_integrate_pos:
	; we reach this point with pvel_x in accumulator

	; integrate low byte
	adc ppos_x
	sta ppos_x
	; integrate high byte
	lda pvel_x + 1
	adc ppos_x + 1
	sta ppos_x + 1

	rts
	
player_tick:
	lda pvel_x
	bmi .pvel_negative
	beq .pvel_zero
; Implicitly: pvel_x is now positive so we need to make acc negative
	ldx #$FF
	stx pacc_x + 1
	stx pacc_x
	jmp .pacc_configured

.pvel_negative:
	ldx #$00
	stx pacc_x + 1
	ldx #$01
	stx pacc_x
	jmp .pacc_configured
	
.pvel_zero:
	ldx #$00
	stx pacc_x + 1
	
.pacc_configured:
	lda control_1
	and #$01
	beq .done_test_right
	ldx #1 << 0
.done_test_right:
	lda control_1
	and #1 << 1
	beq .done_test_left
	ldx #$FF
	stx pacc_x + 1
.done_test_left:
	stx pacc_x
	
	jsr player_integrate
	
; copy to player sprite
	ldx ppos_y + 1
	dex
	; stx SPRITE
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