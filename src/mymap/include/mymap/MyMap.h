/*
 * MyMap.h
 *
 *  Created on: Mar 1, 2018
 *      Author: Arkadiusz Netczuk
 */

#ifndef MYMAP_H_
#define MYMAP_H_

#include "mymap/map_t.h"


/**
 * Reserve memory space.
 */
void *mymap_mmap(map_t *map, void *vaddr, unsigned int size, unsigned int flags, void *o);

/**
 * Release memory.
 */
void mymap_munmap(map_t *map, void *vaddr);

/**
 * Memory initialization.
 */
int mymap_init(map_t *map);

/**
 * Print memory structure.
 */
int mymap_dump(map_t *map);


#endif /* MYMAP_H_ */
