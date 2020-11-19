#ifndef __MY_BLOCK_H
#define __MY_BLOCK_h
#include <stddef.h>
#include "bitmap.h"
#include "list.h"
#include "hugepage.h"
#define BLOCK_SIZE UNIT_SIZE
#define BLOCK_ADDR_MASK ~(BLOCK_SIZE - 1)

typedef struct block_s block_t;
struct block_s
{
    void *start;
    size_t size;
    size_t slab_size;
    bitmap_t *bitmap;
    list_head_t node;
    struct slab_s *slab;
};

int block_init(block_t *block, void *start, struct slab_s *slab);
void *block_alloc_slab(block_t *block);
void block_free_slab(block_t *block, void *p);
block_t *slab_create_block(struct slab_s *slab);
void destroy_block(block_t *block);
#endif