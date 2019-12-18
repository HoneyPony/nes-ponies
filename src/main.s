.include "constants.s"

.segment "ZEROPAGE"
control_1: .res 1
cycle_flag: .res 1

.segment "HEADER"
.byte "NES", 26, 2, 1, 0, 0

.segment "CODE"
.proc irq_handler
	rti
.endproc

.proc nmi_handler
	; OAM DMA update
	pha
	php
	
	lda #$00
	sta OAMADDR
	lda #$02
	sta OAMDMA
	
	inc cycle_flag
	
	plp
	pla
	rti
.endproc

.proc reset_handler
    sei        ; ignore IRQs
    cld        ; disable decimal mode
    ldx #$40
    stx $4017  ; disable APU frame IRQ
    ldx #$ff
    txs        ; Set up stack
    inx        ; now X = 0
    stx $2000  ; disable NMI
    stx $2001  ; disable rendering
    stx $4010  ; disable DMC IRQs

    ; Optional (omitted):
    ; Set up mapper and jmp to further init code here.

    ; The vblank flag is in an unknown state after reset,
    ; so it is cleared here to make sure that @vblankwait1
    ; does not exit immediately.
    bit $2002

    ; First of two waits for vertical blank to make sure that the
    ; PPU has stabilized
vblankwait1:  
    bit $2002
    bpl vblankwait1

    ; We now have about 30,000 cycles to burn before the PPU stabilizes.
    ; One thing we can do with this time is put RAM in a known state.
    ; Here we fill it with $00, which matches what (say) a C compiler
    ; expects for BSS.  Conveniently, X is still 0.
    txa
clrmem1:
    sta $000,x
    sta $100,x
    sta $300,x
    sta $400,x
    sta $500,x
    sta $600,x
    sta $700,x
    inx
    bne clrmem1
    
    lda #$FF
clrmem2:
    sta $200,x
    inx
    bne clrmem2

    ; Other things you can do between vblank waits are set up audio
    ; or set up other mapper registers.
   
vblankwait2:
    bit $2002
    bpl vblankwait2
    
    jmp main
.endproc

.proc readjoy
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

.proc main	  
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
	
loop:
	lda cycle_flag
	beq loop ; branch on zero to skip logic
	dec cycle_flag
	
	; actual logic
	
	jmp loop
.endproc

.segment "VECTORS"
.addr nmi_handler, reset_handler, irq_handler

.segment "CHARS"
.res 8192
.segment "STARTUP"
