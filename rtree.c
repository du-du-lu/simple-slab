#include "rtree.h"
#include "ops.h"
#include "debug.h"
#include "string.h"

int get_index(rtree_key_t key, rtree_map_t *map)
{
    rtree_key_t mask = (1 << map->bits) - 1;
    return (key >> map->offset) & mask;
}

int rtree_insert(rtree_t *rtree, rtree_key_t key, void *data)
{
    int level = 0;
    ASSERT(rtree->root != NULL);
    rtree_node_t *node = rtree->root;
    do
    {
        int index = get_index(key, &rtree->map[level]);
        ASSERT(index < node->sub_num);
        if (level == rtree->level)
        {
            node->data[index] = data;
            break;
        }
        else
        {
            if (node->sub_tree[index] == NULL)
            {
                int sub_num = 1 << (rtree->map[level + 1].bits);
                rtree_node_t *sub_tree = alloc_internal(sizeof(rtree_node_t) + sizeof(rtree_node_t *) * sub_num);
                if (sub_tree == NULL)
                {
                    BUG("cannot alloc new rtree node\n");
                    return -1;
                }
                memset(sub_tree, 0, sizeof(rtree_node_t) + sizeof(rtree_node_t *) * sub_num);
                sub_tree->sub_num = sub_num;
                node->sub_tree[index] = sub_tree;
            }
            node = node->sub_tree[index];
        }
        level++;
    } while (level <= rtree->level);
    return 0;
}

void *rtree_read(rtree_t *rtree, rtree_key_t key, int delete)
{
    int level = 0;
    rtree_node_t *node = rtree->root;
    void *data = NULL;
    do
    {
        if (node == NULL)
        {
            BUG("key never instered,rtree may corrupted\n");
            return NULL;
        }

        int index = get_index(key, &rtree->map[level]);
        if (level == rtree->level)
        {
            data = node->data[index];
            if (data == NULL)
            {
                printf("This key:%lx is empty\n", key);
            }
            if (delete)
            {
                node->data[index] = NULL;
            }
        }
        node = node->sub_tree[index];
        level++;
    } while (level <= rtree->level);
    return data;
}

int rtree_init(rtree_t *rtree, rtree_map_t *map, int level)
{
    int i = 0;
    memset(rtree, 0, sizeof(rtree_t));
    rtree->map = alloc_internal(sizeof(rtree_map_t) * (level + 1));
    if (rtree->map == NULL)
    {
        BUG("cannot alloc rtree map\n");
        return -1;
    }
    for (i = 0; i <= level; i++)
    {
        rtree->map[i] = map[i];
    }
    int sub_tree_num = 1 << (map[0].bits);
    rtree->root = alloc_internal(sizeof(rtree_node_t) + sizeof(rtree_node_t *) * sub_tree_num);
    if (rtree->root == NULL)
    {
        BUG("cannot alloc rtree root tree\n");
        free_internal(rtree->map);
        return -1;
    }
    memset(rtree->root, 0, sizeof(rtree_node_t) + sizeof(rtree_node_t *) * sub_tree_num);
    rtree->root->sub_num = sub_tree_num;
    rtree->level = level;
    return 0;
}