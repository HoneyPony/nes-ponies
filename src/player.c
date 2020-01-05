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
	byte_t direction; /* 0 for normal (right), 1 for flipped */
	byte_t walljump_counter;
	unsigned short jump_amount;
} player;

struct hair_t {
	signed char x0;
	signed char y0;
	
	signed char x1;
	signed char y1;
	
	signed char x2;
	signed char y2;
	
	signed char x3;
	signed char y3;
	
	signed char x4;
	signed char y4;
	
	signed char x5;
	signed char y5;
} hair;

#include "normal-vars.h"

byte_t player_sprite;
byte_t player_hair_front;
byte_t player_hair_back;

byte_t player_colliding() {	
	byte_t result = map_kind((player.x + 0x00) >> 11, (player.y + 0x00) >> 11) |
			map_kind((player.x + 0xF00) >> 11, (player.y + 0x00) >> 11) |
			map_kind((player.x + 0x00) >> 11, (player.y + 0x1700) >> 11) |
			map_kind((player.x + 0xF00) >> 11, (player.y + 0x1700) >> 11);
	return result;
}

byte_t on_floor() {
	return map_kind(player.x >> 11, (player.y + 0x1900) >> 11) |
		map_kind((player.x + 0xF00) >> 11, (player.y + 0x1900) >> 11);
}

byte_t on_wall() {
	short x_off = 0x1100;

	/* I believe we need to use direction because once we're on a wall our
	 * velocity will be zero, more or less. This could lead to weird jank
	 * in rare cases but hopefully it's fine. */
	if(player.direction) {
		x_off = -0x100;
	}
	
	return map_kind((player.x + x_off) >> 11, player.y >> 11) |
		map_kind((player.x + x_off) >> 11, (player.y + 0x1700) >> 11);
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

#define GRAVITY 32
#define JUMP_CORRECTION 15

#define HAIR_SHIFT 4

void test_player_hair() {
	signed char dx = hair.x0 >> HAIR_SHIFT;
	signed char dy = hair.y0 >> HAIR_SHIFT;
	
	byte_t y = (player.y >> 8) - 1;
	byte_t x = (player.x >> 8);
	
	/* hair 0 */
	sprite_ram[player_hair_front] = y + 2 + dy;
	sprite_ram[player_hair_front + 3] = x + 8 + dx;
	
	dx = hair.x1 >> HAIR_SHIFT;
	dy = hair.y1 >> HAIR_SHIFT;

	/* hair 1 */
	sprite_ram[player_hair_back] = y + 2 + dy;
	sprite_ram[player_hair_back + 3] = x + 4 + dx;
	
	/* hair 2 is parented to hair 1 */
	dx += hair.x2 >> HAIR_SHIFT;
	dy += hair.y2 >> HAIR_SHIFT;
	
	/* hair 2 */
	sprite_ram[player_hair_front + 4] = y + 6 + dy;
	sprite_ram[player_hair_front + 7] = x + 5 + dx;
	
	/* Hair 3 not parented to other hair */
	dx = hair.x3 >> HAIR_SHIFT;
	dy = hair.y3 >> HAIR_SHIFT;
	
	/* hair 3 */
	sprite_ram[player_hair_back + 4] = y + 7 + dy;
	sprite_ram[player_hair_back + 7] = x - 3 + dx;
	
	/* hair 5 parented to 4, 4 parented to 3 */
	dx += hair.x4 >> HAIR_SHIFT;
	dy += hair.y4 >> HAIR_SHIFT;
	
	/* hair 4 */
	sprite_ram[player_hair_back + 8] = y + 9 + dy;
	sprite_ram[player_hair_back + 11] = x - 4 + dx;
	
	dx += hair.x5 >> HAIR_SHIFT;
	dy += hair.y5 >> HAIR_SHIFT;
	
	/* hair 5 */
	sprite_ram[player_hair_back + 12] = y + 13 + dy;
	sprite_ram[player_hair_back + 15] = x - 3 + dx;
}

void update_player_sprites() {
	byte_t y = (player.y >> 8) - 1;
	byte_t x1 = (player.x >> 8);
	byte_t x2 = x1;
	
	/* Depending on whether we are facing backwards or not we want to change
	 * which sprite is on the left of the pony and which sprite is on the
	 * right. */
	if(player.direction) {
		x1 += 8;
	}
	else {
		x2 += 8;
	}
	
	sprite_ram[player_sprite] = y;
	sprite_ram[player_sprite + 3] = x1;
	
	sprite_ram[player_sprite + 4] = y;
	sprite_ram[player_sprite + 7] = x2;
	
	y += 8;
	
	sprite_ram[player_sprite + 8] = y;
	sprite_ram[player_sprite + 11] = x1;
	
	sprite_ram[player_sprite + 12] = y;
	sprite_ram[player_sprite + 15] = x2;
	
	y += 8;
	
	sprite_ram[player_sprite + 16] = y;
	sprite_ram[player_sprite + 19] = x1;
	
	sprite_ram[player_sprite + 20] = y;
	sprite_ram[player_sprite + 23] = x2;
	
	/* We need to flip all sprites if facing left; we're re-using x1 and
	 * x2 for this purpose */
	x1 = 0;
	if(player.direction) {
		x1 = 0b01000000;
	}
	
	for(x2 = 2; x2 < 23; x2 += 4) {
		sprite_ram[player_sprite + x2] = x1;
	}
	
	test_player_hair();
}

#define ACCELERATION 24
#define MAX_SLIDE 0x50 /* Half a pixel per second */

#define HAIR_PV_SHIFT 8
#define HAIR_DELTA_SHIFT 5

void hair_clamp(signed char *what) {
	if(*what > -2 && *what < 2) *what = 0;
}

void hair_single(signed char *n, signed char add) {
	if(*n > 0) {
		if(120 - *n < add) {
			*n = 120;
		}
		else *n += add;
		
		//*n -= 4;
	}
	if(*n < 0) {
		if(-120 - *n > add) {
			*n = -120;
		}
		else *n += add;
		
		//*n += 4;
	}
	
	if(*n > -6 && *n < 6) *n = 0;
}

void hair_physics() {
	signed char vx = -player.vx >> 8;
	signed char vy = -player.vy >> 8;
	//vx <<= 5;
	//vy <<= 5;
	
	hair_single(&hair.x0, vx);
	hair_single(&hair.x1, vx);
	hair_single(&hair.x2, vx);
	hair_single(&hair.x3, vx);
	hair_single(&hair.x4, vx);
	hair_single(&hair.x5, vx);
	
	hair_single(&hair.y0, vy);
	hair_single(&hair.y1, vy);
	hair_single(&hair.y2, vy);
	hair_single(&hair.y3, vy);
	hair_single(&hair.y4, vy);
	hair_single(&hair.y5, vy);
	
	/* TODO: add components independently so we only need 1 local var */
	/* root hairs = 0, 1, 3 */
	// hair.x0 += vx;
	// hair.y0 += vy;
	// hair.x1 += vx;
	// hair.y1 += vy;
	// hair.x3 += vx;
	// hair.y3 += vy;
	
	// hair.x2 += vx;
	// hair.y2 += vy;
	// hair.x4 += vx;
	// hair.y4 += vy;
	// hair.x5 += vx;
	// hair.y5 += vy;
	
	// hair.x0 >>= 1;
	// hair.x1 >>= 1;
	// hair.x2 >>= 1;
	// hair.x3 >>= 1;
	// hair.x4 >>= 1;
	// hair.x5 >>= 1;
	
	// hair.y0 >>= 1;
	// hair.y1 >>= 1;
	// hair.y2 >>= 1;
	// hair.y3 >>= 1;
	// hair.y4 >>= 1;
	// hair.y5 >>= 1;
	
	// hair_clamp(&hair.x0);
	// hair_clamp(&hair.x1);
	// hair_clamp(&hair.x2);
	// hair_clamp(&hair.x3);
	// hair_clamp(&hair.x4);
	// hair_clamp(&hair.x5);
	
	// hair_clamp(&hair.y0);
	// hair_clamp(&hair.y1);
	// hair_clamp(&hair.y2);
	// hair_clamp(&hair.y3);
	// hair_clamp(&hair.y4);
	// hair_clamp(&hair.y5);
	
	// /* 2 is parented to 1 */
	// hair.x2 += hair.x1 >> HAIR_DELTA_SHIFT;
	// hair.y2 += hair.y1 >> HAIR_DELTA_SHIFT;
	
	// /* 5 parented to 4 */
	// hair.x5 += hair.x4 >> HAIR_DELTA_SHIFT;
	// hair.y5 += hair.y4 >> HAIR_DELTA_SHIFT;
	
	// /* 4 parented to 3 */
	// hair.x4 += hair.x3 >> HAIR_DELTA_SHIFT;
	// hair.y4 += hair.y3 >> HAIR_DELTA_SHIFT;
}

void player_tick() {	
	/* acceleration x */
	signed char ax = 0;
	/* keeps track if we are pushing the same direction as we are moving (to
	 * slide down wall) */
	byte_t slide_is_input = 0;
	
	/* add old velocity to hair so that it lags behind a frame */
	hair_physics();
	
	if(J_LEFT) {
		ax = -ACCELERATION;
		if(player.direction) slide_is_input = 1;
	}
	else if(J_RIGHT) {
		ax = ACCELERATION;
		if(!player.direction) slide_is_input = 1;
	}
	else {
		ax = -8;
		if(player.vx < 0) ax = 8;
		
		player.vx += ax;
		if(player.vx < 15 && player.vx > -15) player.vx = 0;
	}
	
	/* After walljumping, we don't want to accelerate at all for a few
	 * frames. This gives the player a chance to change which directional
	 * input they're inputting.
	 *
	 * It also prevents us from climbing a singular wall. We can change this
	 * at somepoint if we want.
	 */
	if(!player.walljump_counter)
		player.vx += ax;
	
	if(player.vx > 0x200) player.vx = 0x200;
	if(player.vx < -0x200) player.vx = -0x200;
	
	/* Only update direction once we've reached a good threshold */
	if(player.vx > 0x020) {
		player.direction = 0;
	}
	else if(player.vx < -0x020) {
		player.direction = 1;
	}
	
	player.vy += GRAVITY;
	
	if(slide_is_input && on_wall()) {
		if(player.vy > MAX_SLIDE) player.vy = MAX_SLIDE;
	}
	
	if(J_A) {
		if(player.jump_frames < 5) {
			if(player.air_frames < 4) {
				player.jump_frames = 9;
				player.jump_amount = 0xA0 + JUMP_CORRECTION;
				player.air_frames = 32;
				player.vy = 0;
			}
			
			if(on_wall()) {
				player.jump_frames = 7;
				player.jump_amount = 0x90 + JUMP_CORRECTION;
				player.air_frames = 32;
				player.vy = 0;
				player.walljump_counter = 5;
				/* There appears to be a glitch because we are
				 * not using direction here. TODO */
				if(player.vx < 0) {
					player.vx = 0x200;
				}
				else {
					player.vx = -0x200;
				}
			}
		}
		
		if(player.jump_frames > 0) {
			player.vy -= player.jump_amount;
			--player.jump_frames;
			player.jump_amount -= 16;
		}
		else {
			player.vy -= 4; /* slow gravity */
		}
	}
	else {
		player.jump_frames = 0;
	}
	
	if(player.walljump_counter > 0) {
		--player.walljump_counter;
	}
	
	player_move_with_collisions();
	//player.x += player.vx;
	//player.y += player.vy;
	if(on_floor()) {
		player.air_frames = 0;
	}
	else {
		if(player.air_frames < 32) ++player.air_frames;
	}
	
	update_player_sprites();
}

void player_init() {
	
	player_hair_front = 0x04;
	player_sprite = player_hair_front + 0x08; /* two front hairs */
	player_hair_back = player_sprite + 0x18; /* 6 main sprites */
	
	sprite_ram[player_hair_front] = 0xFF;
	sprite_ram[player_hair_front + 1] = 48;
	sprite_ram[player_hair_front + 2] = 1;
	sprite_ram[player_hair_front + 3] = 0xFF;
	
	sprite_ram[player_hair_front + 4] = 0xFF;
	sprite_ram[player_hair_front + 5] = 50;
	sprite_ram[player_hair_front + 6] = 1;
	sprite_ram[player_hair_front + 7] = 0xFF;
	
	sprite_ram[player_sprite] = 0x8A;
	sprite_ram[player_sprite + 1] = 0;
	sprite_ram[player_sprite + 2] = 0;
	sprite_ram[player_sprite + 3] = 0xB5;
	
	sprite_ram[player_sprite + 4] = 0x8A;
	sprite_ram[player_sprite + 5] = 1;
	sprite_ram[player_sprite + 6] = 0;
	sprite_ram[player_sprite + 7] = 0xB5;
	
	sprite_ram[player_sprite + 8] = 0x8A;
	sprite_ram[player_sprite + 9] = 16;
	sprite_ram[player_sprite + 10] = 0;
	sprite_ram[player_sprite + 11] = 0xB5;
	
	sprite_ram[player_sprite + 12] = 0x8A;
	sprite_ram[player_sprite + 13] = 17;
	sprite_ram[player_sprite + 14] = 0;
	sprite_ram[player_sprite + 15] = 0xB5;
	
	sprite_ram[player_sprite + 16] = 0x8A;
	sprite_ram[player_sprite + 17] = 32;
	sprite_ram[player_sprite + 18] = 0;
	sprite_ram[player_sprite + 19] = 0xB5;
	
	sprite_ram[player_sprite + 20] = 0x8A;
	sprite_ram[player_sprite + 21] = 33;
	sprite_ram[player_sprite + 22] = 0;
	sprite_ram[player_sprite + 23] = 0xB5;
	
	sprite_ram[player_hair_back] = 0xFF;
	sprite_ram[player_hair_back + 1] = 49;
	sprite_ram[player_hair_back + 2] = 1;
	sprite_ram[player_hair_back + 3] = 0xFF;
	
	sprite_ram[player_hair_back + 4] = 0xFF;
	sprite_ram[player_hair_back + 5] = 51;
	sprite_ram[player_hair_back + 6] = 1;
	sprite_ram[player_hair_back + 7] = 0xFF;
	
	sprite_ram[player_hair_back + 8] = 0xFF;
	sprite_ram[player_hair_back + 9] = 52;
	sprite_ram[player_hair_back + 10] = 1;
	sprite_ram[player_hair_back + 11] = 0xFF;
	
	sprite_ram[player_hair_back + 12] = 0xFF;
	sprite_ram[player_hair_back + 13] = 53;
	sprite_ram[player_hair_back + 14] = 1;
	sprite_ram[player_hair_back + 15] = 0xFF;
	
	player.x = 0x8A00;
	player.y = 0x0100;
	player.vx = 1;
	player.vy = 0;
	player.air_frames = 0;
	player.direction = 0;
	
	hair.x0 = 0;
	hair.y0 = 0;
	hair.x1 = 0;
	hair.y1 = 0;
	hair.x2 = 0;
	hair.y2 = 0;
	hair.x3 = 0;
	hair.y3 = 0;
	hair.x4 = 0;
	hair.y4 = 0;
	hair.x5 = 0;
	hair.y5 = 0;
}