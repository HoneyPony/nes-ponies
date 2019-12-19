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

extern byte_t *sprite_ram;

byte_t collision_bitmap[240];
byte_t cbit_ptr = 0;
byte_t cbit_shift = 0;

void out_cb(byte_t k) {
	byte_t r = collision_bitmap[cbit_ptr];
	r = ((r & 0x3) << cbit_shift) | (k << cbit_shift);
	cbit_shift -= 2;
	if(cbit_shift > 6) {
		cbit_shift = 6;
		++cbit_ptr;
	}
}

void set_cb(byte_t x, byte_t y) {
	byte_t x_cel = x >> 2;
	cbit_ptr = x_cel + y << 3;
	/* Given x from 0 to 4, our shift should be:
	 * x == 0 -> 6
	 * x == 1 -> 4
	 * x == 2 -> 2
	 * x == 3 -> 0
	 * this can be calculated with 6 - (x << 1)
	 */
	x = x & 0x3;
	cbit_shift = 6 - (x << 1);
}

const byte_t palette[] = {
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
	//0x01, 0b00100010, 5,
	//0x02, 0b00010110, 5,
	0x00
};

void set_nt(byte_t x, byte_t y) {
	unsigned short address;
	address = x + ((unsigned short)y << 5) + 0x2000;
	PPU.vram.address = address >> 8;
	PPU.vram.address = address;
}

void set_nt_cb(byte_t x, byte_t y) {
	set_nt(x, y);
	set_cb(x, y);
}

void out_nt_cb(byte_t v) {
	out_nt(v);
	out_cb(v);
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
			set_nt_cb(x << 1, y << 1);
			
			out_nt_cb(GR_TL);
			out_nt_cb(GR_T);
			
			len = map[++i];
			while(len) {
				out_nt_cb(GR_T);
				out_nt_cb(GR_T);
				--len;
			}
			out_nt_cb(GR_T);
			out_nt_cb(GR_TR);
			
			set_nt_cb(x << 1, (y << 1) + 1);
			
			out_nt_cb(GR_BL);
			out_nt_cb(GR_B);
			
			len = map[i];
			while(len) {
				out_nt_cb(GR_B);
				out_nt_cb(GR_B);
				--len;
			}
			out_nt_cb(GR_B);
			out_nt_cb(GR_BR);
			
			++i;
		}
		case 0x02: {
			x = map[++i];
			y = x >> 4;
			x = x & 0xF;
			set_nt_cb(x << 1, y << 1);
			
			out_nt(GR_TL);
			out_nt(GR_TR);
			
			len = map[++i];
			
			set_nt_cb(x << 1, ((y + len) << 1) + 1);
			out_nt_cb(GR_L);
			out_nt_cb(GR_R);
			
			set_nt_cb(x << 1, ((y + len) << 1) + 2);
			out_nt_cb(GR_BL);
			out_nt_cb(GR_BR);
			
			while(len) {
				set_nt_cb(x << 1, (y + len) << 1);
				out_nt_cb(GR_L);
				out_nt_cb(GR_R);
				set_nt_cb(x << 1, ((y + len) << 1) - 1);
				out_nt_cb(GR_L);
				out_nt_cb(GR_R);
				--len;
			}
			
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

void init_sprites() {
	byte_t i = 0;
	for(;;) {
		sprite_ram[i] = 0xFF;
		++i;
		if(i == 0) return;
	}
}

void main(void) {
	
	PPU.control = 0;
	PPU.mask = 0;
	
	load_palettes(palette);
	load_map(map_0);
	
	init_sprites();
	
	PPU.vram.address = 0x20;
	PPU.vram.address = 0x00;
	
	PPU.control = 0b10001000;
	PPU.mask = 0b00011110;
	
	for(;;) {

	}
}