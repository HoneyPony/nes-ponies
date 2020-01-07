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

short v_buffer[6];
byte_t wind_buffer[3];

byte_t wind_index;

struct hair_t {
	short x0;
	short y0;
	
	short x1;
	short y1;
	
	short x2;
	short y2;
	
	short x3;
	short y3;
	
	short x4;
	short y4;
	
	short x5;
	short y5;
} hair;

#include "normal-vars.h"

const signed char wind_frames[] = {
	0,
	49,
	90,
	117,
	127,
	117,
	90,
	49,
	0,
	-49,
	-90,
	-117,
	-127,
	-117,
	-90,
	-49


};

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

byte_t map_hair(short x, signed char anchor) {
	if(player.direction) {
		x -= (anchor << 8);
		/* In order to get the flipped anchor position, we extrapolate
		 * from the operation we do in arranging the pony sprites (i.e.
		 * flip between adding 8 and 0 as offsets) in order to get a
		 * formula of 8 pixels - x to flip a given sprite around the
		 * proper pivot. Our numbers now are in subpixels so we have to
		 * use 0x800 for eight pixels.
		 */
		x += 0x800 - (anchor << 8);
	}

	return x >> 8;
}

signed short wind(byte_t buf, byte_t offset) {
	byte_t shift = 2;
	if(player.vx < -0x80 || player.vx > 0x80) {
		shift = 1;
	}
	return (signed short)(wind_frames[((wind_buffer[buf] >> shift) + offset) & 0xF]) << 1;
}

signed char wind2(byte_t buf, byte_t offset) {
	byte_t shift = 2;
	if(player.vx < -0x80 || player.vx > 0x80) {
		shift = 1;
	}
	// if(player.vx < -0x80 || player.vx > 0x80) {
		// return 0;
	// }
	return wind_frames[((wind_buffer[buf] >> shift) + offset) & 0xF];
}

void test_player_hair() {
	/* In order to properly flip the hair sprites, we need to flip them but
	 * keep their motion relative to their anchor position. To do that we
	 * need to subtract their original anchor position to get the relative
	 * position to their anchor, and then add that relative position back
	 * to the flipped anchor position.
	 *
	 * This is accomplished in map_hair() for each x position.
	 */
	 
	 byte_t attributes = 0b00000001;
	 if(player.direction) {
		 attributes = 0b01000001;
	 }
	
	/* hair 0 */
	sprite_ram[player_hair_front]     = ((hair.y0 + wind2(0, 10)) >> 8) - 1;
	sprite_ram[player_hair_front + 2] = attributes;
	sprite_ram[player_hair_front + 3] =
		map_hair(hair.x0 + wind(0, 0), 8);

	/* hair 1 */
	sprite_ram[player_hair_back    ] = ((hair.y1 + wind2(0, 8)) >> 8) - 1;
	sprite_ram[player_hair_back + 2] = attributes;
	sprite_ram[player_hair_back + 3] = 
		map_hair(hair.x1 + wind(0, 5), 4);
	
	/* hair 2 */
	sprite_ram[player_hair_front + 4] = ((hair.y2 + wind2(1, 8)) >> 8) - 1;
	sprite_ram[player_hair_front + 6] = attributes;
	sprite_ram[player_hair_front + 7] = 
		map_hair(hair.x2 + wind(1, 5), 5);
	
	/* hair 3 */
	sprite_ram[player_hair_back + 4] = ((hair.y3 + wind2(0, 6)) >> 8) - 1;
	sprite_ram[player_hair_back + 6] = attributes;
	sprite_ram[player_hair_back + 7] = 
		map_hair(hair.x3 + wind2(0, 3), -3);
	
	/* hair 4 */
	sprite_ram[player_hair_back + 8] = ((hair.y4 + wind2(1, 6)) >> 8) - 1;
	sprite_ram[player_hair_back + 10] = attributes;
	sprite_ram[player_hair_back + 11] =
		map_hair(hair.x4 + wind2(1, 3), -4);
	
	/* hair 5 */
	sprite_ram[player_hair_back + 12] = ((hair.y5 + wind2(2, 6)) >> 8) - 1;
	sprite_ram[player_hair_back + 14] = attributes;
	sprite_ram[player_hair_back + 15] = 
		map_hair(hair.x5 + wind(2, 3), -3);
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

void hair_physics() {
	/* root hairs = 0, 1, 3 */
	hair.x0 += v_buffer[0];
	hair.y0 += v_buffer[1];
	
	hair.x1 += v_buffer[0];
	hair.y1 += v_buffer[1];
	
	hair.x3 += v_buffer[0];
	hair.y3 += v_buffer[1];
	
	/* hairs 2 and 4 are parented */
	hair.x2 += v_buffer[2];
	hair.y2 += v_buffer[3];
	
	hair.x4 += v_buffer[2];
	hair.y4 += v_buffer[3];
	
	/* hair 5 is parented to 4, which is itself parented */
	hair.x5 += v_buffer[4];
	hair.y5 += v_buffer[5];
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
		if(player.vx < 15 && player.vx > -15) {
			player.vx = 0;
		}
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

				/* We use direction here because otherwise if
				 * we are facing towards a left wall we can
				 * reach a velocity of zero and then repeatedly
				 * right-wall jump off of the wall.
				 *
				 * In theory the player can reach a wall
				 * backwards by reaching it with very low
				 * velocity in the direction opposite they are
				 * facing, and then get infinite wall jumps that
				 * way, but we'll leave that as a potential
				 * speedrunning trick rather than do something
				 * to fix it because it should be pretty hard to
				 * do and not encountered in normal gameplay.
				 *
				 * However, even that seems to already be
				 * impossible because on_wall() only checks in
				 * the direction we are facing. So it seems the
				 * last paragraph is probably completely wrong,
				 * and so I'll leave it in as possible
				 * documentation in case such a feature does
				 * exist but some testing shows it doesn't seem
				 * to work. (wall jumping when facing the wrong
				 * way seems to just propel you in the direction
				 * you are facing).
				 */
				if(player.direction) {
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
	
	v_buffer[4] = v_buffer[2];
	v_buffer[5] = v_buffer[3];
	wind_buffer[2] = wind_buffer[1];
	
	v_buffer[2] = v_buffer[0];
	v_buffer[3] = v_buffer[1];
	wind_buffer[1] = wind_buffer[0];
	
	v_buffer[0] = player.vx;
	v_buffer[1] = player.vy;
	++wind_buffer[0];
	
	//++wind_index;
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
	
	hair.x0 = player.x + (8 << 8);
	hair.y0 = player.y + (2 << 8);
	hair.x1 = player.x + (4 << 8);
	hair.y1 = player.y + (2 << 8);
	hair.x2 = player.x + (5 << 8);
	hair.y2 = player.y + (6 << 8);
	hair.x3 = player.x - (3 << 8);
	hair.y3 = player.y + (7 << 8);
	hair.x4 = player.x - (4 << 8);
	hair.y4 = player.y + (9 << 8);
	hair.x5 = player.x - (3 << 8);
	hair.y5 = player.y + (13 << 8);
	
	v_buffer[0] = 0;
	v_buffer[1] = 0;
	v_buffer[2] = 0;
	v_buffer[3] = 0;
	v_buffer[4] = 0;
	v_buffer[5] = 0;
	
	wind_buffer[0] = 0;
	wind_buffer[1] = 0;
	wind_buffer[2] = 0;
	
	wind_index = 0;
	
	prng_seed = 0x137d;
}