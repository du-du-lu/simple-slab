#ifndef __BUDDY_H
#define __BUDDY_H
#include "types.h"
#include "debug.h"
#define L_CHILD_INDEX(x) (2 * x + 1)
#define R_CHILD_INDEX(x) (2 * x + 2)
#define PARENT_INDEX(x) ((x - 1) / 2)
#define IS_POWER_OF_2(x) (!((x) & ((x)-1)))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

/*if odd, add one
  if even, sub one
  to get sibling index
*/
#define SIBLING_INDEX(x) (x - 1 + 2 * (x & 1))

#define IS_LEFT_CHILD(x) (!!(x & 1))
#define IS_RIGHT_CHILD(x) ((x != = 0) && (!(x & 1)))

typedef struct buddy_s buddy_t;
struct buddy_s
{
  int order;
  char heap[];
};
buddy_t *buddy_create(char order);
void buddy_free(buddy_t *buddy, unsigned index);
unsigned buddy_alloc(buddy_t *buddy, unsigned root, char order);
unsigned u32_align(unsigned index);
void mark_free(buddy_t *buddy, u32 index);
char get_order_of(u32 n);
#endif