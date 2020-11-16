#ifndef __MY_SLAB_H
#define __MY_SLAB_H
#include <stddef.h>
#include <pthread.h>
#include "list.h"
#include "rtree.h"
typedef struct slab_s slab_t;
struct slab_s
{
    size_t slab_size;
    size_t block_size;
    pthread_mutex_t block_lock;
    list_head_t head;
    struct block_s *current_block;
    int free_count;
};
void *slab_alloc(slab_t *slab);
void slab_init(slab_t *slab, size_t slab_size);
void slab_recycle_fn(slab_t *slab);
pthread_mutex_t rtree_lock;
rtree_t block_rtree;
#endif