#ifndef __RTREE_H
#define __RTREE_H
#include "types.h"
#define MAX_LEVEL 10
typedef struct rtree_map_s rtree_map_t;

typedef struct rtree_node_s rtree_node_t;

typedef struct rtree_s rtree_t;

typedef u64 rtree_key_t;

struct rtree_node_s
{
    int sub_num;
    union
    {
        struct rtree_node_s *sub_tree[0];
        void *data[0];
    };
};

struct rtree_map_s
{
    int offset;
    int bits;
};

struct rtree_s
{
    rtree_map_t *map;
    int level;
    rtree_node_t *root;
};
int rtree_insert(rtree_t *rtree, rtree_key_t key, void *data);
void *rtree_read(rtree_t *rtree, rtree_key_t key, int delete);
int rtree_init(rtree_t *rtree, rtree_map_t *map, int level);
int get_index(rtree_key_t key, rtree_map_t *map);
#endif