#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <dev_alloc.h>

#define QUEUE_MAX_SIZE 10
#define TEST_ROUND 10000000
struct queue_s
{
    void *buf[QUEUE_MAX_SIZE];
    volatile int productor;
    char a[64];
    volatile int consumer;
};
typedef struct queue_s queue_t;

void enqueue(void *p, queue_t *q)
{
    volatile int consumer;
redo:
    consumer = __atomic_load_n(&q->consumer, __ATOMIC_ACQUIRE);
    if (q->productor == consumer + QUEUE_MAX_SIZE - 1)
    {
        goto redo;
    }
    q->buf[q->productor % QUEUE_MAX_SIZE] = p;
    __atomic_add_fetch(&q->productor, 1, __ATOMIC_RELEASE);
}

void *dequeue(queue_t *q)
{
    volatile int productor;
redo:
    productor = __atomic_load_n(&q->productor, __ATOMIC_ACQUIRE);
    if (q->consumer == productor)
    {
        goto redo;
    }
    void *p = q->buf[q->consumer % QUEUE_MAX_SIZE];
    __atomic_add_fetch(&q->consumer, 1, __ATOMIC_RELEASE);
    return p;
}

struct test
{
    pthread_t productor_pid;
    pthread_t consumer_pid;
    queue_t q;
};
void *productor_fn(void *data)
{

    for (int i = 0; i < TEST_ROUND; i++)
    {
        int *p = dev_alloc(10 * 1024);
        if (p == NULL)
        {
            printf("cannot alloc new\n");
            return NULL;
        }
        *p = i;
        queue_t *q = (queue_t *)data;
        enqueue(p, q);
    }
    return NULL;
}

void *consumer_fn(void *data)
{
    queue_t *q = (queue_t *)data;
    for (int i = 0; i < TEST_ROUND; i++)
    {
        void *p = dequeue(q);
        if (*(int *)p != i)
        {
            printf("find fault!\n");
            return NULL;
        }
        dev_free(p);
    }
    return NULL;
}

struct test test_array[10];

int main()
{
    for (int i = 0; i < 10; i++)
    {
        pthread_create(&test_array[i].productor_pid, NULL, productor_fn, &test_array[i].q);
        pthread_create(&test_array[i].consumer_pid, NULL, consumer_fn, &test_array[i].q);
    }
    for (int i = 0; i < 10; i++)
    {
        pthread_join(test_array[i].productor_pid, NULL);
        pthread_join(test_array[i].consumer_pid, NULL);
    }
    return 0;
}
