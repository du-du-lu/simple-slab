#ifndef __ALLOC_H
#define __ALLOC_H
#include <stddef.h>
#include "slab.h"
slab_t **slab_pool;
int slab_num;
size_t slab_size_max;
#endif