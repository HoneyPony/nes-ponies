#include "main.h"
#include "palette.h"
#include "map.h"
#include "player.h"

#include <ines.h>

MAPPER_PRG_ROM_KB(32);
MAPPER_CHR_ROM_KB(8);

void init_sprites() {
	byte_t i = 0;
	for(;;) {
		sprite_ram[i] = 0xFF;
		++i;
		if(i == 0) return;
	}
}

const byte_t game_palette[] = {
	0x31, 0x2A, 0x38, 0x30,
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

const byte_t map_1[] = {
	M_HLINE, 0b11100000, 14,
	//M_HLINE, 0b11000000, 12,
	//M_HLINE, 0b10110000, 10,
	M_VLINE, 0b00000000, 3,
	M_VLINE, 0b00000100, 3,
	M_DONE
};

extern void read_controller();
extern volatile byte_t cycle_flag;

void test_left() {
	PPU.control = 0;
	PPU.mask = 0;
	
	/* black bg */
	//load_bg(0x0F);
	load_map(map_0);
	//load_bg(game_palette[0]);
	
	SYNC_PPU();
	PPU.vram.address = 0x20;
	PPU.vram.address = 0x00;
}

void test_right() {
	PPU.control = 0;
	PPU.mask = 0;
	
	//load_bg(0x0F);
	load_map(map_1);
	//load_bg(game_palette[0]);
	
	SYNC_PPU();
	PPU.vram.address = 0x20;
	PPU.vram.address = 0x00;
}

void init_nametable_attributes() {
	SYNC_PPU();
	PPU.vram.address = 0x23;
	PPU.vram.address = 0xC0;
	for(byte_t i = 0; i < 64; ++i) {
		PPU.vram.data = 0;
	}
}

int main(void) {
	PPU.control = 0;
	PPU.mask = 0;
	
	load_palettes(game_palette);
	init_nametable_attributes();
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