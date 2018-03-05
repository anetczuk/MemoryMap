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

#ifndef MEMORYAREA_H_
#define MEMORYAREA_H_

#include <stddef.h>                 /// size_t


typedef enum {
    READ = (1 << 0),
    WRITE = (1 << 1),
    EXEC = (1 << 2)
} MemoryFlag;


typedef struct {
    ///MemoryFlag flags;                /// unused
    size_t start;
    size_t end;
    ///unsigned char* data;             /// unused
} MemoryArea;


/// ====================================================


static inline MemoryArea memory_create( const size_t start, const size_t size ) {
    MemoryArea area;
    area.start = start;
    area.end   = start + size;
    return area;
}

static inline size_t memory_size( const MemoryArea* area ) {
	return area->end - area->start;
}

void memory_init( MemoryArea* area );

void memory_release( MemoryArea* area );

/**
 * Return 0 if successful, otherwise false.
 */
int memory_fitBetween(const MemoryArea* first, const MemoryArea* second, MemoryArea* check);

void memory_fitAfter(const MemoryArea* segment, MemoryArea* check);


#endif /* MEMORYAREA_H_ */
