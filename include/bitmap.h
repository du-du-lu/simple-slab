#ifndef __MY_BITMAP_H
#define __MY_BITMAP_H
#include "types.h"
#include "stdbool.h"
struct bitmap_s
{
    u32 bits;
    u64 bit_array[];
};
typedef struct bitmap_s bitmap_t;
bitmap_t *create_bitmap(u32 bits);
u32 get_bit(bitmap_t *bitmap);
void put_bit(bitmap_t *bitmap, u32 offset);
bool check_bitmap_empty(bitmap_t *bitmap);
bool check_bitmap_full(bitmap_t *bitmap);
#endif