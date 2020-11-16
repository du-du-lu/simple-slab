#include "types.h"
#include "ops.h"
#include "bitmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bitmap_t *create_bitmap(u32 bits)
{
    bitmap_t *ret;
    u32 align_bits = ((bits + 63) / 64) * 64;
    u32 total_size = align_bits / 8;
    u32 valid_size = bits / 8;

    ret = alloc_internal(sizeof(bitmap_t) + total_size);
    if (ret)
    {
        memset(ret->bit_array, 0xff, valid_size);
        memset((char *)(ret->bit_array) + valid_size, 0, total_size - valid_size);
        ret->bits = bits;
    }
    return ret;
}

bool check_bitmap_empty(bitmap_t *bitmap)
{
    u64 *p;
    u64 *endp;
    p = bitmap->bit_array;
    endp = bitmap->bit_array + ((bitmap->bits + 63) / 64);
    while (*p == 0)
    {
        p++;
        if (p > endp)
        {
            return true;
        }
    }
    return false;
}

bool check_bitmap_full(bitmap_t *bitmap)
{
    u64 *p;
    u64 *endp;
    p = bitmap->bit_array;
    endp = bitmap->bit_array + ((bitmap->bits + 63) / 64);
    while (*p == 1)
    {
        p++;
        if (p > endp)
        {
            return true;
        }
    }
    return false;
}

u32 get_bit(bitmap_t *bitmap)
{
    u64 *p;
    u64 *endp;
    u64 origin;
    u64 new;
    u64 mask;
    u32 num_u64;
    u32 bit_offset;

    endp = bitmap->bit_array + ((bitmap->bits + 63) / 64);
first:
    p = bitmap->bit_array;
    while (*p == 0)
    {
        p++;
        if (p > endp)
        {
            return -1U;
        }
    }
    num_u64 = p - bitmap->bit_array;
second:
    origin = *p;
    if (origin == 0)
    {
        goto first;
    }
    bit_offset = __builtin_clzl(origin);
    mask = ~(1UL << (63 - bit_offset));
    bit_offset += num_u64 * 64;
    new = origin &mask;
    if (!__sync_bool_compare_and_swap(p, origin, new))
    {
        goto second;
    }
    return bit_offset;
}

void put_bit(bitmap_t *bitmap, u32 offset)
{
    u64 origin;
    u64 new;
    u64 mask;
    u64 *p;
    if (offset > bitmap->bits)
    {
        return;
    }
    mask = 1UL << (63 - offset % 64);
    p = bitmap->bit_array + offset / 64;
redo:
    origin = *p;
    new = origin | mask;
    if (!__sync_bool_compare_and_swap(p, origin, new))
    {
        goto redo;
    }
}