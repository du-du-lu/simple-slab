#include <stddef.h>
#include "slab.h"
#include <sched.h>
#include "rtree.h"
#include <pthread.h>
#include "block.h"
#include "alloc.h"
#include "dev_alloc.h"
#include "debug.h"

#define RECYCLE_COUNT 100

slab_t *get_slab(size_t size, int cpu)
{
    int low_index = 0;
    int high_index = slab_num - 1;
    slab_t *slab;
    ASSERT(size >= slab_pool[cpu][low_index].slab_size && size <= slab_pool[cpu][high_index].slab_size);
    while (low_index < high_index - 1)
    {
        int mid_index = (low_index + high_index) / 2;
        if (slab_pool[cpu][mid_index].slab_size < size)
        {
            low_index = mid_index;
        }
        else
        {
            high_index = mid_index;
        }
    }
    if (slab_pool[cpu][low_index].slab_size == size)
    {
        slab = &slab_pool[cpu][low_index];
    }
    else
    {
        slab = &slab_pool[cpu][high_index];
    }
    return slab;
}

void *dev_alloc(size_t size)
{
    int cpu = sched_getcpu();
    if (cpu == -1)
    {
        cpu = 0;
    }
    slab_t *slab = get_slab(size, cpu);
    return slab_alloc(slab);
}

void dev_free(void *p)
{
    block_t *block;
    void *block_start = (void *)((u64)p & BLOCK_ADDR_MASK);
    pthread_mutex_lock(&rtree_lock);
    block = rtree_read(&block_rtree, (rtree_key_t)block_start, 0);
    pthread_mutex_unlock(&rtree_lock);
    if (block)
    {
        block_free_slab(block, p);
    }
    if (block->slab->free_count++ == 2 * (block->slab->block_size / block->slab->slab_size))
    {
        slab_recycle_fn(block->slab);
        block->slab->free_count = 0;
    }
    return;
}
