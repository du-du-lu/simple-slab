#ifndef __MY_OPS_H
#define __MY_OPS_H
#include <stddef.h>
void *alloc_internal(size_t size);
void free_internal(void *p);
#endif