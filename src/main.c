#include "main.h"
#include "palette.h"
#include "map.h"
#include "player.h"

void init_sprites() {
	byte_t i = 0;
	for(;;) {
		sprite_ram[i] = 0xFF;
		++i;
		if(i == 0) return;
	}
}

const byte_t game_palette[] = {
	0x31, 0x06, 0x16, 0x1A,
	0x31, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F,
	
	0x31, 0x03, 0x23, 0x34,
	0x2A, 0x03, 0x2A, 0x3a,
	0x2A, 0x2A, 0x2A, 0x2A,
	0x2A, 0x2A, 0x2A, 0x2A
};

const byte_t map_0[] = {
	M_HLINE, 0b11100000, 14,
	//M_HLINE, 0b11000000, 12,
	//M_HLINE, 0b10110000, 10,
	M_VLINE, 0b00000000, 9,
	M_VLINE, 0b00000100, 9,
	M_DONE
};

extern void read_controller();
extern byte_t cycle_flag;

void main(void) {
	PPU.control = 0;
	PPU.mask = 0;
	
	load_palettes(game_palette);
	load_map(map_0);
	
	init_sprites();
	
	SYNC_PPU();
	PPU.vram.address = 0x20;
	PPU.vram.address = 0x00;
	
	PPU.control = 0b10001000;
	PPU.mask = 0b00011110;
	
	player_init();
	
	for(;;) {
		if(cycle_flag) {
			cycle_flag = 0;
			read_controller();
			player_tick();
		}
	}
}