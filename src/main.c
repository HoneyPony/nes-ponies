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
	
	0x31, 0x30, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x30
};

const byte_t map_0[] = {
	M_HLINE, 0b11010000, 14,
	M_DONE
};

extern void read_controller();

void main(void) {
	PPU.control = 0;
	PPU.mask = 0;
	
	load_palettes(game_palette);
	load_map(map_0);
	
	init_sprites();
	
	PPU.vram.address = 0x20;
	PPU.vram.address = 0x00;
	
	PPU.control = 0b10001000;
	PPU.mask = 0b00011110;
	
	player_init();
	
	for(;;) {
		read_controller();
		player_tick();
	}
}