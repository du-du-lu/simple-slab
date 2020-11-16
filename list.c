#include "list.h"
void __list_add(list_head_t *new, list_head_t *prev, list_head_t *next)
{
    prev->next = new;
    new->prev = prev;
    new->next = next;
    next->prev = new;
};

void list_add_head(list_head_t *head, list_head_t *new)
{
    __list_add(new, head, head->next);
}

void list_add_tail(list_head_t *head, list_head_t *new)
{
    __list_add(new, head->prev, head);
}

void list_del(list_head_t *node)
{
    list_head_t *prev = node->prev;
    list_head_t *next = node->next;
    prev->next = next;
    next->prev = prev;
}

void list_head_init(list_head_t *head)
{
    head->prev = head->next = head;
}