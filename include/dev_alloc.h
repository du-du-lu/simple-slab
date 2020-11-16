#ifndef __DEV_ALLOC_H
#define __DEV_ALLOC_H
#include "stddef.h"
void *dev_alloc(size_t size);
void dev_free(void *p);
#endif
