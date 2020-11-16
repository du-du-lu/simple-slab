#ifndef __BUG_H
#define __BUG_H
#include <stdio.h>
#include <stdlib.h>
#define BUG(x) printf("%s\n", x)
#define WARN(x) printf("%s\n", x)
#define ASSERT(x)                 \
    do                            \
    {                             \
        ((x) == 0) ? abort() : 0; \
    } while (0)
#endif