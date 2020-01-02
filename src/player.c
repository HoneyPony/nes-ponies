#include "main.h"
#include "map.h"

struct player_t {
	unsigned short x;
	unsigned short y;
	short vx;
	short vy;
} player;

byte_t player_sprite = 0x04;

byte_t player_colliding() {
	byte_t x = (player.x + 4) >> 11;
	byte_t y = (player.y + 4) >> 11;
	
	return map_kind(x, y) | map_kind(x + 1, y) | map_kind(x, y + 1) | map_kind(x + 1, y + 1);
}

void player_tick() {	
	if(J_LEFT) player.vx = -0x0F0;
	else if(J_RIGHT) player.vx = 0xF0;
	else player.vx = 0;
	
	if(J_UP) player.vy = -0x0F0;
	else if(J_DOWN) player.vy = 0xF0;
	else player.vy = 0;
	
	player.x += player.vx;
	player.y += player.vy;
	
	//player.vy += 3;
	
	if(player_colliding()) {
		//player.y -= 0x200;
	}

	{
		byte_t y = (player.y >> 8) - 1;
		byte_t x = (player.x >> 8);
		byte_t mx = player.x >> 11;
		byte_t my = player.y >> 11;
		
		sprite_ram[player_sprite] = y;
		sprite_ram[player_sprite + 2] = map_kind(mx, my);
		sprite_ram[player_sprite + 3] = x;
		
		sprite_ram[player_sprite + 4] = y + 8;
		sprite_ram[player_sprite + 6] = map_kind(mx, my + 1);
		sprite_ram[player_sprite + 7] = x;
		
		sprite_ram[player_sprite + 8] = y;
		sprite_ram[player_sprite + 10] = map_kind(mx + 1, my);
		sprite_ram[player_sprite + 11] = x + 8;
		
		sprite_ram[player_sprite + 12] = y + 8;
		sprite_ram[player_sprite + 14] = map_kind(mx + 1, my + 1);
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