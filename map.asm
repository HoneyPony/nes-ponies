	.data
map_0: db 0xFF
	
map_table: dw map_0

	.code

MAP_ADDR = $300
ALGO_ADDR = $700

map_instruction:
	rts

load_map:
	ldx #$00
	
load_map_ld_instruction:
	lda $FEFE, x
	beq .done_loading
	jsr map_instruction
	inx
	jmp load_map_ld_instruction
	
.done_loading:
	rts
load_map_end

LOAD_MAP_SIZE = (load_map_end - load_map)

; Loads a map from the map_table, based on x register
load_map_from_table:
	ldy #$00
	dey
	
.next_byte:
	iny
	lda load_map,y
	sta ALGO_ADDR,y
	
	cpy #LOAD_MAP_SIZE
	bne .next_byte
	
.load_addr:
	lda map_table,x
	sta ALGO_ADDR + (load_map_ld_instruction - load_map) + 2
	inx
	lda map_table,x
	sta ALGO_ADDR + (load_map_ld_instruction - load_map) + 1
	
	jsr ALGO_ADDR
	rts