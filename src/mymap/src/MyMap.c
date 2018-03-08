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

#include <stddef.h>         /// NULL
#include <stdio.h>          /// printf


#include <memorymap/RBTree.h>



struct map_element {
    RBTree root;
};



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

int mymap_release(map_t *map) {
    //TODO: implement
    return -1;
}

/**
 * Print memory structure.
 */
int mymap_dump(map_t *map) {
    //TODO: implement
    ///printf("%s", "xxxx");
    return -1;
}

size_t mymap_size(const map_t *map) {
    //TODO: implement
    return 0;
}

int mymap_isValid(const map_t *map) {
    //TODO: implement
    return -1;
}
