/// MIT License
///
/// Copyright (c) 2017 Arkadiusz Netczuk <dev.arnet@gmail.com>
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include "mymap/MyMap.h"

#include <stddef.h>                     /// NULL
#include <stdio.h>                      /// printf
#include <stdlib.h>                     /// free


#include <memorymap/RBTree.h>



typedef struct map_root {
    RBTree tree;
} map_element;



/**
 * Reserve memory space.
 * Fields 'flags' and 'o' not supported for now.
 */
void *mymap_mmap(map_t *map, void *vaddr, unsigned int size, unsigned int flags, void* o) {
    (void) flags; /* unused */
    (void) o; /* unused */

    if (map == NULL) {
        return NULL;
    }
    if (map->root == NULL) {
        return NULL;
    }
    return tree_mmap( &(map->root->tree), vaddr, size );
}

/**
 * Release memory.
 */
void mymap_munmap(map_t *map, void *vaddr) {
    if (map == NULL) {
        return ;
    }
    if (map->root == NULL) {
        return ;
    }
    tree_munmap( &(map->root->tree), vaddr );
}

/**
 * Memory initialization.
 */
int mymap_init(map_t *map) {
    if (map == NULL) {
        return -1;
    }
    map->root = calloc(1, sizeof(map_element) );
    return tree_init( &(map->root->tree) );
}

int mymap_release(map_t *map) {
    if (map == NULL) {
        return -1;
    }
    if (map->root == NULL) {
        return -2;
    }
    const int ret = tree_release( &(map->root->tree) );

    free(map->root);
    map->root = NULL;

    return ret;
}

/**
 * Print memory structure.
 */
int mymap_dump(map_t *map) {
    if (map == NULL) {
        return 0;
    }
    if (map->root == NULL) {
        return 0;
    }
    tree_print( &(map->root->tree) );
    return 0;
}

size_t mymap_size(const map_t *map) {
    if (map == NULL) {
        return 0;
    }
    if (map->root == NULL) {
        return 0;
    }
    return tree_size( &(map->root->tree) );
}

void *mymap_startAddress(const map_t *map) {
    if (map == NULL) {
        return NULL;
    }
    if (map->root == NULL) {
        return NULL;
    }
    return (void *)tree_startAddress( &(map->root->tree) );
}

void *mymap_endAddress(const map_t *map) {
    if (map == NULL) {
        return NULL;
    }
    if (map->root == NULL) {
        return NULL;
    }
    return (void *)tree_endAddress( &(map->root->tree) );
}

int mymap_isValid(const map_t *map) {
    if (map == NULL) {
        return 0;
    }
    if (map->root == NULL) {
        return -1;
    }
    return tree_isValid( &(map->root->tree) );
}
