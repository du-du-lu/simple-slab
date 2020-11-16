#ifndef __LIST_H
#define __LIST_H
struct list_head_s
{
    struct list_head_s *next;
    struct list_head_s *prev;
};
typedef struct list_head_s list_head_t;
void list_add_head(list_head_t *head, list_head_t *new);
void list_add_tail(list_head_t *head, list_head_t *new);
void list_del(list_head_t *node);
void list_head_init(list_head_t *head);

#define container_of(ptr, type, member) ({                              \
        void *__mptr = (void *)(ptr);                                   \
        ((type *)(__mptr - offsetof(type, member))); })
#endif