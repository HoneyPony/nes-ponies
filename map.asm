	.data
map_0: db 0x00
	
map_table: dw map_0

	.code

MAP_ADDR = $300
	

; Loads a map from the map_table, based on x register
load_map_from_table:
	lda map_table, x
	sta map_load_address + 1, x
	inx
	lda map_table, x
	sta map_load_address, x
	
	stx #$00
.map_instruction:
	lda (map_load_address, x)
	beq .done_loading
	
	cmp #$01
	beq .map_instruction_1
	
	jmp .map_instruction
	
.map_instruction_1:

	

	jmp .map_instruction
	
.done_loading:
	rts