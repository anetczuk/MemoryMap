/*
 * MyMap.c
 *
 *  Created on: Mar 1, 2018
 *      Author: bob
 */

#include "mymap/MyMap.h"

#include <stddef.h>         /// NULL


/**
 * Reserve memory space.
 */
void *mymap_mmap(map_t *map, void *vaddr, unsigned int size, unsigned int flags, void *o) {
    //TODO: implement
    return NULL;
}

/**
 * Release memory.
 */
void mymap_munmap(map_t *map, void *vaddr) {
    //TODO: implement
}

/**
 * Memory initialization.
 */
int mymap_init(map_t *map) {
    //TODO: implement
    return -1;
}

/**
 * Print memory structure.
 */
int mymap_dump(map_t *map) {
    //TODO: implement
    return -1;
}
