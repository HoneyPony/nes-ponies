#include "main.h"
#include "map.h"

struct player_t {
	unsigned short x;
	unsigned short y;
	short vx;
	short vy;
	
	byte_t air_frames;
} player;

byte_t player_sprite = 0x04;

byte_t player_colliding() {	
	byte_t result = map_kind((player.x + 0x00) >> 11, (player.y + 0x00) >> 11) |
			map_kind((player.x + 0xF00) >> 11, (player.y + 0x00) >> 11) |
			map_kind((player.x + 0x00) >> 11, (player.y + 0xF00) >> 11) |
			map_kind((player.x + 0xF00) >> 11, (player.y + 0xF00) >> 11);
	return result;
}

void player_move_with_collisions() {
	
	short tvx = player.vx;
	short tvy = player.vy;
	
	byte_t iterations = 1;
	byte_t depth = 0;
	
	byte_t y_flag = 0;
	byte_t x_flag = 0;
	
	for(;;) {
		player.y += tvy;
		
		if(player_colliding()) {
			y_flag = 1;
			player.y -= tvy;
			tvy = tvy >> 1;
			
			iterations <<= 1;
		}
		else {
			--iterations;
			if(iterations == 0) break;
		}
		
		if(++depth == 2) break;
	}
	
	depth = 0;
	iterations = 1;
	for(;;) {
		player.x += tvx;
		
		if(player_colliding()) {
			x_flag = 1;
			player.x -= tvx;
			tvx = tvx >> 1;
			
			iterations <<= 1;
		}
		else {
			--iterations;
			if(iterations == 0) break;
		}
		
		if(++depth == 2) break;
	}
	
	

	player.vx = tvx;
	player.vy = tvy;
	
	if(x_flag) {
		player.x &= 0xFF00;
	}
	if(y_flag) {
		player.y &= 0xFF00;
		player.air_frames = 0;
	}
	else {
		if(player.air_frames < 32) ++player.air_frames;
	}
	
}

void player_tick() {	
	signed char ax = 0;
	if(J_LEFT) {
		ax = -16;
	}
	else if(J_RIGHT) {
		ax = 16;
	}
	else {
		ax = -8;
		if(player.vx < 0) ax = 8;
		
		player.vx += ax;
		if(player.vx < 15 && player.vx > -15) player.vx = 0;
	}
	player.vx += ax;
	if(player.vx > 0x200) player.vx = 0x200;
	if(player.vx < -0x200) player.vx = -0x200;
	
	player.vy += 24;
	if(player.air_frames < 4 && J_A) {
		player.vy -= 0x400;
		player.air_frames = 32;
	}
	
	player_move_with_collisions();
	//player.x += player.vx;
	//player.y += player.vy;
	
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
	player.y = 0x0100;
	player.vx = 1;
	player.vy = 0;
	player.air_frames = 0;
}