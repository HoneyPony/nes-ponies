JOYPAD1 = 4016

.global controller
.global read_controller

.section .bss
controller: .fill 1

.section .text
read_controller:
    lda #01
    ; While the strobe bit is set, buttons will be continuously reloaded.
    ; This means that reading from JOYPAD1 will only return the state of the
    ; first button: button A.
    sta JOYPAD1
    sta controller
    lsr a        ; now A is 0
    ; By storing 0 into JOYPAD1, the strobe bit is cleared and the reloading stops.
    ; This allows all 8 buttons (newly reloaded) to be read from JOYPAD1.
    sta JOYPAD1
loop:
    lda JOYPAD1
    lsr a	       ; bit 0 -> Carry
    rol controller  ; Carry -> bit 0; bit 7 -> Carry
    bcc loop
    rts