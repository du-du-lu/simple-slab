#include <sys/mman.h>
#include "hugepage.h"

void *index_to_address(u32 index, huge_page_t *p)
{
    u32 level_split_num;
    u32 level_start_index;
    u32 offset;
    void *ret;
    if (index == 0)
    {
        ret = p->start;
    }
    else
    {
        level_split_num = 1 << (31 - __builtin_clz(index + 1));
        level_start_index = level_split_num - 1;
        offset = (HUGE_PAGE_SIZE / level_split_num) * (index - level_start_index);
        ret = (void *)((u64)(p->start) + offset);
    }
    return ret;
}

void *dev_alloc_units(int units)
{
    list_head_t *iterator = NULL;
    char unit_order = get_order_of(units);
redo:
    iterator = huge_page_list.next;
    pthread_mutex_lock(&huge_page_list_lock);
    while (iterator != &huge_page_list)
    {
        huge_page_t *p = container_of(iterator, huge_page_t, node);
        u32 index = buddy_alloc(p->buddy, 0, unit_order);
        if (index == -1U)
        {
            continue;
        }
        else
        {

            pthread_mutex_unlock(&huge_page_list_lock);

            return index_to_address(index, p);
        }
    }
    pthread_mutex_unlock(&huge_page_list_lock);
    huge_page_t *new_page = alloc_internal(sizeof(huge_page_t));
    if (new_page == NULL)
    {
        return NULL;
    }
    new_page->start = mmap(NULL, HUGE_PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memdevfd, 0);
    if (new_page->start == NULL)
    {
        free_internal(new_page);
    }
    new_page->buddy = buddy_create(get_order_of(HUGE_PAGE_SIZE / UNIT_SIZE));
    if (new_page->buddy == NULL)
    {
        free_internal(new_page->buddy);
        return NULL;
    }
    pthread_mutex_lock(&huge_page_list_lock);
    list_add_head(&huge_page_list, &new_page->node);
    pthread_mutex_unlock(&huge_page_list_lock);
    goto redo;
    return NULL;
}

u32 address_to_index(void *p, int units, huge_page_t *page)
{
    u64 offset = (u64)p - (u64)(page->start);
    int ordermax = get_order_of(UNITS_PER_HUGE_PAGE);
    int order = get_order_of(units);
    int split_num = 1 << (ordermax - order);
    int index_offset = offset / (HUGE_PAGE_SIZE / split_num);
    u32 level_first_index = split_num - 1;
    return level_first_index + index_offset;
}

void dev_free_units(void *p, int units)
{
    void *huge_page_start = (void *)((u64)p & HUGE_PAGE_ADDR_MASK);
    ASSERT(units & (units - 1) == 0);
    pthread_mutex_lock(&huge_page_list_lock);
    list_head_t *iterator = huge_page_list.next;
    while (iterator != &huge_page_list)
    {
        list_head_t *next = iterator->next;
        huge_page_t *tmp = container_of(iterator, huge_page_t, node);
        if (tmp->start == huge_page_start)
        {
            u32 index = address_to_index(p, units, tmp);
            buddy_free(tmp->buddy, index);
            if (tmp->buddy->order == tmp->buddy->heap[0])
            {
                munmap(tmp->start, HUGE_PAGE_SIZE);
                free_internal(tmp->buddy);
                list_del(&tmp->node);
                free_internal(tmp);
                break;
            }
        }
        iterator = next;
    }
    pthread_mutex_unlock(&huge_page_list_lock);
    ASSERT(iterator != &huge_page_list);
}