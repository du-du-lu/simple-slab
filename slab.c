#include "slab.h"
#include "rtree.h"
#include "bitmap.h"
#include "block.h"

void slab_init(slab_t *slab, size_t slab_size)
{
    slab->block_size = BLOCK_SIZE;
    slab->slab_size = slab_size;
    slab->current_block = NULL;
    pthread_mutex_init(&slab->block_lock, NULL);
    slab->free_count = 0;
    list_head_init(&slab->head);
}

void *slab_alloc(slab_t *slab)
{
    void *ret = NULL;
    block_t *new_block = NULL;
bitmap_alloc:
    ret = block_alloc_slab(slab->current_block);
    if (ret == NULL)
    {
        pthread_mutex_lock(&slab->block_lock);
        list_head_t *iterator = slab->head.next;
        while (iterator != &slab->head)
        {
            new_block = container_of(iterator, block_t, node);
            if (!check_bitmap_allused(new_block->bitmap))
            {
                break;
            }
            else
            {
                new_block = NULL;
            }
            iterator = iterator->next;
        }
        if (new_block == NULL)
        {
            new_block = slab_create_block(slab);
            if (new_block == NULL)
            {
                pthread_mutex_unlock(&slab->block_lock);
                return NULL;
            }
            pthread_mutex_lock(&rtree_lock);
            rtree_insert(&block_rtree, (rtree_key_t)(new_block->start), (void *)new_block);
            pthread_mutex_unlock(&rtree_lock);
        }
        block_t *block = slab->current_block;
        slab->current_block = new_block;
        if (block != NULL)
        {
            list_add_tail(&slab->head, &block->node);
        }
        pthread_mutex_unlock(&slab->block_lock);
        goto bitmap_alloc;
    }
    return ret;
}

void slab_recycle_fn(slab_t *slab)
{
    pthread_mutex_lock(&slab->block_lock);
    list_head_t *iterator = slab->head.next;
    while (iterator != &slab->head)
    {
        block_t *block = container_of(iterator, block_t, node);
        list_head_t *next = iterator->next;
        if (check_bitmap_unused(block->bitmap))
        {
            list_del(iterator);
            destroy_block(block);
        }
        iterator = next;
    }
    pthread_mutex_unlock(&slab->block_lock);
}