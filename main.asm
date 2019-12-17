	.include "constants.asm"
	.include "header.asm"

	.zp
; Controller bytes
control_1: ds 1
control_2: ds 1

; cycle flag
cycle_flag: ds 1

	.data
	.bank 0
	.org $C000
	
	.include "palettes.asm"

	.code
	.bank 0
	.org $C200

	.include "nmi.asm"
	.include "reset.asm"
	.include "player.asm"
	.include "map.asm"
	
; Loads a complete set of palettes, including background and sprite palettes.
; The palettes are loaded starting at `palettes`, plus the x index register.
; This enables 8 completely different sets of palettes to be loaded based on the 
; initial x index.
load_palettes_by_x:
	lda PPUSTATUS
	lda #$3F
	sta PPUADDR
	lda #$00
	sta PPUADDR
	
	ldy #$00
	
	.loop:
	
		lda palettes,x
		sta PPUDATA
		inx
		iny
		
		cpy #$1F
		bne .loop
	
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
.loop:
	lda JOYPAD1
	lsr a	       ; bit 0 -> Carry
	rol control_1  ; Carry -> bit 0; bit 7 -> Carry
	bcc .loop
	rts
	
idle:
	jsr readjoy
	
	lda cycle_flag
	beq idle ; branch on zero to skip logic
	dec cycle_flag
	jsr player_tick
	
	jmp idle
	
main:
	; Load initial palettes
	ldx #00
	jsr load_palettes_by_x

	jsr player_init
	
	ldx #$00
	jsr load_map_from_table
	
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
