JOYPAD1 = $4016

.export _controller
.export _read_controller

.segment "BSS"
_controller: .res 1

.segment "CODE"
.proc _read_controller

    lda #$01
    ; While the strobe bit is set, buttons will be continuously reloaded.
    ; This means that reading from JOYPAD1 will only return the state of the
    ; first button: button A.
    sta JOYPAD1
    sta _controller
    lsr a        ; now A is 0
    ; By storing 0 into JOYPAD1, the strobe bit is cleared and the reloading stops.
    ; This allows all 8 buttons (newly reloaded) to be read from JOYPAD1.
    sta JOYPAD1
loop:
    lda JOYPAD1
    lsr a	       ; bit 0 -> Carry
    rol _controller  ; Carry -> bit 0; bit 7 -> Carry
    bcc loop
    rts

.endproc