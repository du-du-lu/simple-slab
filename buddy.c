#include "buddy.h"
#include <stddef.h>
#include "debug.h"
#include <stdlib.h>
#include "types.h"
#include <stdbool.h>
u32 u32_align(u32 index)
{
    if (IS_POWER_OF_2(index))
        return index;
    index |= (index >> 1);
    index |= (index >> 2);
    index |= (index >> 4);
    index |= (index >> 8);
    index |= (index >> 16);
    return index + 1;
}

char get_order_of(u32 n)
{
    if (!IS_POWER_OF_2(n))
    {
        n = u32_align(n);
    }
    else if (n == 0)
    {
        n = 1;
    }
    return __builtin_ctz(n);
}

char index2order(buddy_t *buddy, u32 index)
{
    u32 a = u32_align(index + 2);
    int level = __builtin_ctz(a);
    char origin_order = buddy->order - (level - 1);
    return origin_order;
}

buddy_t *buddy_create(char order)
{
    int i = 0;
    /*2n-1是二叉树节点的数量*/
    u32 n = 1 << order;
    size_t size = (2 * n - 1) * sizeof(char);
    buddy_t *buddy = malloc(size + sizeof(buddy_t));
    if (buddy == NULL)
    {
        return NULL;
    }
    buddy->order = order;
    order = order + 1;
    for (i = 0; i < size; i++)
    {
        if (IS_POWER_OF_2(i + 1))
        {
            order = order - 1;
        }
        buddy->heap[i] = order;
    }

    return buddy;
}

/*内部接口，order一定小于root拥有的order*/
u32 find_fit(buddy_t *buddy, u32 root, char order)
{
    ASSERT(order <= buddy->heap[root]);
    ASSERT(order >= 0);

    u32 index = root;
    u32 n = 1 << (buddy->order);
    u32 index_max = 2 * n - 1;
    while (buddy->heap[index] >= order)
    {
        u32 lc = L_CHILD_INDEX(index);
        u32 rc = R_CHILD_INDEX(index);
        if (lc > index_max || rc > index_max)
        {
            break;
        }

        if (buddy->heap[lc] >= order)
        {
            index = lc;
        }
        else if (buddy->heap[rc] >= order)
        {
            index = rc;
        }
        else
        {
            break;
        }
    }
    return index;
}

void mark_alloced(buddy_t *buddy, u32 index)
{
    ASSERT(index <= ((1 << (buddy->order + 1)) - 1));
    ASSERT(buddy->heap[index] >= 0);
    buddy->heap[index] = -1;
}

void mark_free(buddy_t *buddy, u32 index)
{
    ASSERT(index <= ((1 << (buddy->order + 1)) - 1));
    ASSERT(buddy->heap[index] == -1);
    char order = index2order(buddy, index);
    buddy->heap[index] = order;
}

void update_parents(buddy_t *buddy, u32 index)
{
    ASSERT(index <= ((1 << (buddy->order + 1)) - 1));
    char origin_order = index2order(buddy, index);
    char computed_parent_order = 0;
    bool need_update = true;

    while (need_update && index != 0)
    {
        u32 sibling_index = SIBLING_INDEX(index);
        u32 parent_index = PARENT_INDEX(index);
        char current_order = buddy->heap[index];
        char sibling_order = buddy->heap[sibling_index];
        if (current_order == origin_order && sibling_order == origin_order)
        {
            computed_parent_order = origin_order + 1;
        }
        else
        {
            computed_parent_order = MAX(current_order, sibling_order);
        }

        if (computed_parent_order == buddy->heap[parent_index])
        {
            need_update = false;
        }
        else
        {
            need_update = true;
            buddy->heap[parent_index] = computed_parent_order;
        }
        index = parent_index;
        origin_order = origin_order + 1;
    }
}

u32 buddy_alloc(buddy_t *buddy, u32 root, char order)
{
    if (buddy->heap[root] < order)
    {
        return -1U;
    }
    u32 index = find_fit(buddy, root, order);
    mark_alloced(buddy, index);
    update_parents(buddy, index);
    return index;
}

void buddy_free(buddy_t *buddy, u32 index)
{
    mark_free(buddy, index);
    update_parents(buddy, index);
    return;
}