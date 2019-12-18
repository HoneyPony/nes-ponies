#include <nes.h>

#define SYNC_PPU (void)(PPU.status)

#define GR_TL 0x00
#define GR_TR 0x01
#define GR_BL 0x02
#define GR_BR 0x03
#define GR_T  0x04
#define GR_B  0x05
#define GR_L  0x06
#define GR_R  0x07

#define SKY   0xFF

#define out_nt(tile) (PPU.vram.data = (tile))

typedef unsigned char byte_t;

const byte_t palette[] = {
	0x31, 0x06, 0x16, 0x1A,
	0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F,
	
	0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F
};

const byte_t map_0[] = {
	0x01, 0x00, 5,
	0x00
};

void set_nt(byte_t x, byte_t y) {
	unsigned short address;
	address = x + ((unsigned short)y << 5) + 0x2000;
	PPU.vram.address = address >> 8;
	PPU.vram.address = address;
}

void load_map(const byte_t *map) {
	byte_t i = 0;
	byte_t x;
	byte_t y;
	byte_t len;
	
	PPU.control = 0;
	PPU.mask = 0;
	
	PPU.vram.address = 0x20;
	PPU.vram.address = 0x00;
	for(x = 0; x < 4; ++x) {
		for(i = 0; i < 240; ++i) {
			out_nt(SKY);
		}
	}
	
	i = 0;
	
	for(;;) {
		switch(map[i]) {
		default:
		case 0x00:
			goto done_loading_map;
		case 0x01: {
			x = map[++i];
			y = x >> 4;
			x = x & 0xF;
			set_nt(x << 1, y << 1);
			
			out_nt(GR_TL);
			out_nt(GR_T);
			
			len = map[++i];
			while(len) {
				out_nt(GR_T);
				out_nt(GR_T);
				--len;
			}
			out_nt(GR_T);
			out_nt(GR_TR);
			
			set_nt(x << 1, (y << 1) + 1);
			
			out_nt(GR_BL);
			out_nt(GR_B);
			
			len = map[i];
			while(len) {
				out_nt(GR_B);
				out_nt(GR_B);
				--len;
			}
			out_nt(GR_B);
			out_nt(GR_BR);
			
			++i;
		}
			
		}
	}
	
done_loading_map:
	PPU.control = 0b10001000;
	PPU.mask = 0b00011110;
}

void load_palettes(const byte_t *palettes) {
	unsigned char i = 0;
	
	(void)(PPU.status);
	
	PPU.vram.address = 0x3F;
	PPU.vram.address = 0x00;
	
	for(i = 0; i < 32; ++i) {
		PPU.vram.data = palettes[i];
	}
	
}

void main(void) {
	
	
	PPU.control = 0;
	PPU.mask = 0;
	
	load_palettes(palette);
	load_map(map_0);
	
	PPU.control = 0b10001000;
	PPU.mask = 0b00011110;
	
	for(;;) {
	}
}