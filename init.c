
#include "rtree.h"
#include "hugepage.h"
#include "debug.h"
#include "alloc.h"
#include "ops.h"
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include "third-party/cJSON/cJSON.h"
#define MEMDEV_PATH "/dev/memdev0"
#define CONFIG_JSON_PATH "/etc/alloc.json"
/*
用户地址空间为0-0x00007fffffffffff
min block size 为1M
也就是从20-46位是有效的
*/
rtree_map_t blcok_rtree_map[3] = {
    {.bits = 9, .offset = 38},
    {.bits = 9, .offset = 29},
    {.bits = 9, .offset = 20}};

size_t parse_size(const char *buf)
{
    char n[100];
    int i = 0;
    int j = 0;
    size_t result = 0;
    while (*buf != 'M' && *buf != 'm' && *buf != 'k' && *buf != 'k')
    {
        ASSERT(*buf <= '9' && *buf >= '0');
        n[i++] = *buf - '0';
        buf++;
    }
    while (j < i)
    {
        result = result * 10 + n[j];
        j++;
    }
    if (*buf == 'M' || *buf == 'm')
    {
        result = result * 1024 * 1024;
    }
    else
    {
        result = result * 1024;
    }
    return result;
}

void init_slab_pool(void)
{
    char *config_buf = NULL;
    struct stat config_stat;
    int config_fd = 0;
    cJSON *slab_array;
    int i, j;
    int cpu_num = sysconf(_SC_NPROCESSORS_CONF);
    ASSERT(stat(CONFIG_JSON_PATH, &config_stat) != -1);
    config_buf = alloc_internal(config_stat.st_size);
    ASSERT(config_buf != NULL);
    config_fd = open(CONFIG_JSON_PATH, O_RDONLY);
    ASSERT(config_fd >= 0);
    read(config_fd, config_buf, config_stat.st_size);

    cJSON *root = cJSON_Parse(config_buf);
    ASSERT(root != NULL);
    slab_array = cJSON_GetObjectItem(root, "config");
    ASSERT(slab_array->type == cJSON_Array);
    slab_num = cJSON_GetArraySize(slab_array);
    slab_pool = alloc_internal(sizeof(slab_t *) * cpu_num);
    ASSERT(slab_pool != NULL);
    for (i = 0; i < cpu_num; i++)
    {
        slab_pool[i] = alloc_internal(sizeof(slab_t) * slab_num);
        ASSERT(slab_pool[i] != NULL);
        for (j = 0; j < slab_num; j++)
        {
            cJSON *item = cJSON_GetArrayItem(slab_array, j);
            size_t slab_size = parse_size(item->valuestring);
            slab_init(&slab_pool[i][j], slab_size);
        }
    }
}

__attribute__((constructor)) void lib_init(void)
{
    memdevfd = open(MEMDEV_PATH, O_RDWR);
    ASSERT(memdevfd >= 0);
    init_slab_pool();
    rtree_init(&block_rtree, blcok_rtree_map, 2);
    pthread_mutex_init(&rtree_lock, NULL);
    list_head_init(&huge_page_list);
    pthread_mutex_init(&huge_page_list_lock, NULL);
}
