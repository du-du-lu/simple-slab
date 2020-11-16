#include "rtree.h"
#include "stdio.h"
rtree_map_t map[3] = {
    {.offset = 8, .bits = 4},
    {.offset = 4, .bits = 4},
    {.offset = 0, .bits = 4}};

rtree_t rtree;
int main()
{
    rtree_init(&rtree, map, 2);
    for (u64 i = 0; i < 0xffaUL; i++)
    {
        if (rtree_insert(&rtree, (rtree_key_t)i, (void *)i))
        {
            printf("fail to insert key:0x%lx\n", i);
            return -1;
        }
    }
    for (u64 i = 0; i < 0xffaUL; i++)
    {
        void *data = rtree_read(&rtree, (rtree_key_t)i, 1);
        if ((rtree_key_t)data != (rtree_key_t)i)
        {
            printf("rtree read not match\n");
            printf("data:%lx, i:%lx\n", (rtree_key_t)data, i);
            return -1;
        }
    }
    return 0;
}