#ifndef __HUGE_PAGE_H
#define __HUGE_PAGE_H
#include "buddy.h"
#include "list.h"
#include <pthread.h>
#include "ops.h"
#define HUGE_PAGE_SIZE (1UL * 1024 * 1024 * 1024)
#define HUGE_PAGE_ADDR_MASK (~(HUGE_PAGE_SIZE - 1))
#define UNIT_SIZE (1UL * 1024 * 1024)
#define UNITS_PER_HUGE_PAGE (HUGE_PAGE_SIZE / UNIT_SIZE)
#define PAGE_ORDER 10
struct huge_page_s
{
    list_head_t node;
    buddy_t *buddy;
    void *start;
    u64 huge_page_size;
};
typedef struct huge_page_s huge_page_t;
list_head_t huge_page_list;
pthread_mutex_t huge_page_list_lock;
int memdevfd;
void *dev_alloc_units(int units);
void dev_free_units(void *p, int units);
#endif