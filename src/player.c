#include "main.h"

struct player_t {
	unsigned short x;
	unsigned short y;
	short vx;
	short vy;
} player;

byte_t player_sprite = 0x04;

void player_tick() {	
	if(J_LEFT) player.vx = -10;
	else if(J_RIGHT) player.vx = 10;
	else player.vx = 0;
	
	player.x += player.vx;

	{
		byte_t y = (player.y >> 8) - 1;
		byte_t x = (player.x >> 8);
		
		sprite_ram[player_sprite] = y;
		sprite_ram[player_sprite + 3] = x;
		
		sprite_ram[player_sprite + 4] = y + 8;
		sprite_ram[player_sprite + 7] = x;
		
		sprite_ram[player_sprite + 8] = y;
		sprite_ram[player_sprite + 11] = x + 8;
		
		sprite_ram[player_sprite + 12] = y + 8;
		sprite_ram[player_sprite + 15] = x + 8;
	}
}

void player_init() {
	sprite_ram[player_sprite] = 0x8A;
	sprite_ram[player_sprite + 1] = 0;
	sprite_ram[player_sprite + 2] = 0;
	sprite_ram[player_sprite + 3] = 0xB5;
	
	sprite_ram[player_sprite + 4] = 0x8A;
	sprite_ram[player_sprite + 5] = 1;
	sprite_ram[player_sprite + 6] = 0;
	sprite_ram[player_sprite + 7] = 0xB5;
	
	sprite_ram[player_sprite + 8] = 0x8A;
	sprite_ram[player_sprite + 9] = 2;
	sprite_ram[player_sprite + 10] = 0;
	sprite_ram[player_sprite + 11] = 0xB5;
	
	sprite_ram[player_sprite + 12] = 0x8A;
	sprite_ram[player_sprite + 13] = 3;
	sprite_ram[player_sprite + 14] = 0;
	sprite_ram[player_sprite + 15] = 0xB5;
	
	player.x = 0x8A00;
	player.y = 0xB500;
	player.vx = 1;
	player.vy = 0;
}