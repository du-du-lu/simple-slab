#include "bitmap.h"
#include <pthread.h>
#include <stdio.h>
#define THREAD_NUM 1
#define BITS_PER_THREAD 1024 * 1024
struct tdata_s
{
    bitmap_t *bitmap;
    pthread_t tid;
};
typedef struct tdata_s tdata_t;
tdata_t data[THREAD_NUM];
void *thread_fn(void *data)
{
    int i = 0;
    u32 tmp = 0;
    tdata_t *tdata = data;
    bitmap_t *bitmap = tdata->bitmap;
    while (i < BITS_PER_THREAD)
    {
        tmp = get_bit(bitmap);
        if (tmp == -1U)
        {
            printf("fail to get bit");
            break;
        }
        else
        {
            printf("tid:0x%x,bit:%u\n", tdata->tid, tmp);
        }
        i++;
    }
    return NULL;
}

int main()
{
    int i = 0;
    bitmap_t *bitmap = create_bitmap(BITS_PER_THREAD * THREAD_NUM);

    for (i = 0; i < THREAD_NUM; i++)
    {
        data[i].bitmap = bitmap;
        pthread_create(&(data[i].tid), NULL, thread_fn, &data[i]);
    }
    for (i = 0; i < THREAD_NUM; i++)
    {
        pthread_join(data[i].tid, NULL);
    }
    for (i = 0; i < BITS_PER_THREAD * THREAD_NUM / 64; i++)
    {
        if (*(bitmap->bit_array + i) != 0)
        {
            printf("cannot get all bits\n");
            return -1;
        }
    }
    return 0;
}