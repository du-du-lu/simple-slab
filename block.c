#include "block.h"
#include "bitmap.h"
#include "ops.h"
#include "slab.h"

int block_init(block_t *block, void *start, struct slab_s *slab)
{
    block->size = BLOCK_SIZE;
    block->slab_size = slab->slab_size;
    u32 slab_num = block->size / block->slab_size;
    ASSERT(slab_num > 0);
    block->bitmap = create_bitmap(slab_num);
    if (block->bitmap == NULL)
    {
        return -1;
    }
    block->start = start;
    list_head_init(&block->node);
    block->slab = slab;
    return 0;
}

block_t *slab_create_block(struct slab_s *slab)
{
    block_t *block = alloc_internal(sizeof(block_t));
    if (block == NULL)
    {
        return NULL;
    }
    void *start = dev_alloc_units(BLOCK_SIZE / UNIT_SIZE);
    if (start == NULL)
    {
        free_internal(block);
        return NULL;
    }
    if (block_init(block, start, slab))
    {
        free_internal(block);
        dev_free_units(start, BLOCK_SIZE / UNIT_SIZE);
        return NULL;
    }
    return block;
}

void destroy_block(block_t *block)
{
    pthread_mutex_lock(&rtree_lock);
    rtree_read(&block_rtree, (rtree_key_t)(block->start), 1);
    pthread_mutex_unlock(&rtree_lock);
    dev_free_units(block->start, BLOCK_SIZE / UNIT_SIZE);
    free_internal(block);
    return;
}

void *block_alloc_slab(block_t *block)
{
    u32 offslab;
    if (block == NULL)
    {
        return NULL;
    }
    offslab = get_bit(block->bitmap);
    if (offslab == -1U)
    {
        return NULL;
    }
    return (void *)((u64)(block->start) + offslab * block->slab_size);
}

void block_free_slab(block_t *block, void *p)
{
    u64 offset = (u64)p - (u64)(block->start);
    u32 slab = offset / block->slab_size;
    put_bit(block->bitmap, slab);
    return;
}