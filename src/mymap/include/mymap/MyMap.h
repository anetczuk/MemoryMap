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

#ifndef MYMAP_H_
#define MYMAP_H_

#include <stddef.h>                           /// NULL, size_t


typedef struct {
    struct map_element* root;                /// pimpl idiom
} map_t;


/// ==================================================


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

int mymap_release(map_t *map);

/**
 * Print memory structure.
 */
int mymap_dump(map_t *map);

size_t mymap_size(const map_t *map);

int mymap_isValid(const map_t *map);

#endif /* MYMAP_H_ */
