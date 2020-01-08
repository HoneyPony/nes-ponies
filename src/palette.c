#include "main.h"

void load_palettes(const byte_t *palettes) {
	unsigned char i = 0;
	
	(void)(PPU.status);
	
	SYNC_PPU();
	PPU.vram.address = 0x3F;
	PPU.vram.address = 0x00;
	
	for(i = 0; i < 32; ++i) {
		PPU.vram.data = palettes[i];
	}
	
}

void load_bg(byte_t bg) {
	SYNC_PPU();
	PPU.vram.address = 0x3F;
	PPU.vram.address = 0x00;
	
	PPU.vram.data = bg;
	
	SYNC_PPU();
	PPU.vram.address = 0x3F;
	PPU.vram.address = 0x10;
	
	PPU.vram.data = bg;
}