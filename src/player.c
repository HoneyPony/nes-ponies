#include "main.h"
#include "map.h"

#include "zeropage-vars.h"

struct player_t {
	unsigned short x;
	unsigned short y;
	short vx;
	short vy;
	
	byte_t air_frames;
	byte_t jump_frames;
	unsigned short jump_amount;
} player;

#include "normal-vars.h"

byte_t player_sprite;

byte_t player_colliding() {	
	byte_t result = map_kind((player.x + 0x00) >> 11, (player.y + 0x00) >> 11) |
			map_kind((player.x + 0xF00) >> 11, (player.y + 0x00) >> 11) |
			map_kind((player.x + 0x00) >> 11, (player.y + 0xF00) >> 11) |
			map_kind((player.x + 0xF00) >> 11, (player.y + 0xF00) >> 11);
	return result;
}

byte_t on_floor() {
	return map_kind(player.x >> 11, (player.y + 0x1100) >> 11) |
		map_kind((player.x + 0xF00) >> 11, (player.y + 0x1100) >> 11);
}

#define COL_LOOP(delta, cmp, axis)\
for(;;){\
	if(delta cmp v) {\
		player.axis += v;\
		if(player_colliding()) {\
			player.axis -= v;\
			player.v##axis = 0;\
		}\
		break;\
	}\
	else {\
		player.axis += delta;\
		if(player_colliding()) {\
			player.axis -= delta;\
			player.v##axis = v;\
			break;\
		}\
		v -= 0x800;\
	}\
}\

void player_move_with_collisions() {
	short v;
	
	if(player.vx > 0x1000) player.vx = 0x1000;
	if(player.vx < -0x1000) player.vx = -0x1000;
	if(player.vy > 0x1000) player.vy = 0x1000;
	if(player.vy < -0x1000) player.vy = -0x1000;
	
	v = player.vx;
	
	if(v < 0) {
		COL_LOOP(-0x800, <, x)
	}
	else {
		COL_LOOP(0x800, >, x)
	}
	
	v = player.vy;
	
	if(v < 0) {
		COL_LOOP(-0x800, <, y)
	}
	else {
		COL_LOOP(0x800, >, y)
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
	if(J_A) {
		if(player.air_frames < 4 && player.jump_frames == 0) {
			player.jump_frames = 30;
			player.jump_amount = 0x100;
			player.air_frames = 32;
			player.vy = 0;
		}
		
		if(player.jump_frames > 0) {
			player.vy -= player.jump_amount;
			--player.jump_frames;
			player.jump_amount -= 10;
		}
	}
	else {
		player.jump_frames = 0;
	}
	
	player_move_with_collisions();
	//player.x += player.vx;
	//player.y += player.vy;
	if(on_floor() && player.jump_frames < 25) {
		player.air_frames = 0;
		player.jump_frames = 0;
	}
	else {
		if(player.air_frames < 32) ++player.air_frames;
	}
	
	{
		byte_t y = (player.y >> 8) - 1;
		byte_t x = (player.x >> 8);
		byte_t mx = player.x >> 11;
		byte_t my = player.y >> 11;
		byte_t f = on_floor();
		
		sprite_ram[player_sprite] = y;
		sprite_ram[player_sprite + 2] = f;//map_kind(mx, my);
		sprite_ram[player_sprite + 3] = x;
		
		sprite_ram[player_sprite + 4] = y + 8;
		sprite_ram[player_sprite + 6] = f;//map_kind(mx, my + 1);
		sprite_ram[player_sprite + 7] = x;
		
		sprite_ram[player_sprite + 8] = y;
		sprite_ram[player_sprite + 10] = f;//map_kind(mx + 1, my);
		sprite_ram[player_sprite + 11] = x + 8;
		
		sprite_ram[player_sprite + 12] = y + 8;
		sprite_ram[player_sprite + 14] = f;//map_kind(mx + 1, my + 1);
		sprite_ram[player_sprite + 15] = x + 8;
	}
}

void player_init() {
	player_sprite = 0x04;
	
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