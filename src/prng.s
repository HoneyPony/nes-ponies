; prng
;
; Returns a random 8-bit number in A (0-255), clobbers Y (0).
;
; Requires a 2-byte value on the zero page called "seed".
; Initialize seed to any value except 0 before the first call to prng.
; (A seed value of 0 will cause prng to always return 0.)
;
; This is a 16-bit Galois linear feedback shift register with polynomial $0039.
; The sequence of numbers it generates will repeat after 65535 calls.
;
; Execution time is an average of 125 cycles (excluding jsr and rts)

.segment "ZEROPAGE"
_prng_seed: .res 2       ; initialize 16-bit seed to any value except 0
_prng_out: .res 1

.segment "CODE"
.proc _prng
	ldy #8     ; iteration count (generates 8 bits)
	lda _prng_seed+0
:
	asl        ; shift the register
	rol _prng_seed+1
	bcc :+
	eor #$39   ; apply XOR feedback whenever a 1 bit is shifted out
:
	dey
	bne :--
	sta _prng_seed+0
	cmp #0     ; reload flags
	
	sta _prng_out
	
	rts
.endproc

.export _prng_seed
.export _prng_out
.export _prng