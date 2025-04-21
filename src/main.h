#ifndef MAIN_H
#define MAIN_H

/* Picture Processing Unit */
struct __ppu {
    unsigned char control;
    unsigned char mask;                 /* color; show sprites, background */
      signed char volatile const status;
    struct {
        unsigned volatile char address;
        unsigned volatile char data;
    } sprite;
    unsigned char scroll;
    struct {
        unsigned volatile char address;
        unsigned volatile char data;
    } vram;
};
#define PPU             (*(struct __ppu*)0x2000)

#define SYNC_PPU() __asm__ volatile("lda $2002" ::: "a", "memory")

typedef unsigned char byte_t;

extern volatile byte_t controller;

//#include "zeropage-vars.h"

extern volatile unsigned short prng_seed;
extern volatile byte_t prng_out;

//#pragma zpsym ("prng_seed")
//#pragma zpsym ("prng_out")

#include "normal-vars.h"

extern void prng();

#define J_RIGHT  (controller & 0x01)
#define J_LEFT   (controller & 0x02)
#define J_DOWN   (controller & 0x04)
#define J_UP     (controller & 0x08)
#define J_START  (controller & 0x10)
#define J_SELECT (controller & 0x20)
#define J_B      (controller & 0x40)
#define J_A      (controller & 0x80)

extern volatile byte_t sprite_ram[256];

#endif