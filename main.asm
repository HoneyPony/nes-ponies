.include "constants.asm"
.include "header.asm"

.segment "ZEROPAGE"
; Controller bytes
control_1: ds 1
control_2: ds 1

; cycle flag
cycle_flag: ds 1

.include "nmi.asm"
.include "reset.asm"
	
.proc readjoy:
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
.endproc
	
.proc idle
	jsr readjoy
	
	lda cycle_flag
	beq idle ; branch on zero to skip logic
	dec cycle_flag
	jsr game_tick
	
	jmp idle
.endproc
	
.proc main
	jsr game_init
	
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
.endproc

.segment "VECTORS"
.addr nmi_handler
.addr reset_handler
.addr 0

.segment "CHARS"
.incbin "graphics.chr"
