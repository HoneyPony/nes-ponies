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

.section .zp.data
prng_seed: .fill 2       ; initialize 16-bit seed to any value except 0
prng_out: .fill 1

.section .text
prng:
	ldy #8     ; iteration count (generates 8 bits)
	lda prng_seed+0
1:
	asl        ; shift the register
	rol prng_seed+1
	bcc 2
	eor #39   ; apply XOR feedback whenever a 1 bit is shifted out
2:
	dey
	bne 1
	sta prng_seed+0
	cmp #0     ; reload flags
	
	sta prng_out
	
	rts

.global prng_seed
.global prng_out
.global prng