#ifndef MAP_H
#define MAP_H

#include "main.h"

void load_map(const byte_t *map);
byte_t map_kind(byte_t x, byte_t y);

#define M_DONE  0x00
#define M_HLINE 0x01
#define M_VLINE 0x02

#endif