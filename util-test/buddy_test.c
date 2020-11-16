#include "buddy.h"
#include <stdio.h>

int main()
{
    unsigned index;
    buddy_t *buddy = buddy_create(10);
    index = buddy_alloc(buddy, 0, 2);
    printf("order 2 index :%d\n", index);
    index = buddy_alloc(buddy, 0, 3);
    printf("order 3 index :%d\n", index);
    buddy_free(buddy, 255);
    index = buddy_alloc(buddy, 0, 3);
    printf("order 3 index :%d\n", index);
    index = buddy_alloc(buddy, 0, 9);
    printf("order 9 index :%d\n", index);
    //    buddy_free(buddy, 255);
    buddy_free(buddy, 128);
    buddy_free(buddy, 127);
    buddy_free(buddy, 2);
    free(buddy);
    return 0;
}