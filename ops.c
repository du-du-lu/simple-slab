#include "ops.h"
#include "stdlib.h"
#include "hugepage.h"
void *alloc_internal(size_t size)
{
    return malloc(size);
}

void free_internal(void *p)
{
    free(p);
}