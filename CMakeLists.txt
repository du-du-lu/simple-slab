cmake_minimum_required(VERSION 3.0.0)
project(devalloc VERSION 0.1.0)

SET(SRC_LIST alloc.c bitmap.c block.c buddy.c hugepage.c init.c list.c ops.c rtree.c slab.c)
SET(INCLUDE_PATH include)
SET(THIRD_PARTY_SRC_LIST third-party/cJSON/cJSON.c)
SET(THIRD_PARTY_INCLUDE third-party/cJSON)
INCLUDE_DIRECTORIES(${INCLUDE_PATH} ${THIRD_PARTY_INCLUDE})

ADD_LIBRARY(devalloc_static STATIC ${SRC_LIST} ${THIRD_PARTY_SRC_LIST})


