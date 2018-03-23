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

#include "memorymap/MemoryArea.h"

#include <assert.h>
#include <stdio.h>                             /// printf


void memory_print( const MemoryArea* area ) {
    if (area == NULL) {
        printf("NULL");
    } else {
        printf("%zu,%zu", area->start, area->end);
    }
}

int memory_compare( const MemoryArea* area1, const MemoryArea* area2 ) {
    assert( area1!=NULL );
    assert( area2!=NULL );
    if ( area1->end <= area2->start ) {
        return -1;
    }
    if ( area1->start >= area2->end ) {
        return 1;
    }
    return 0;
}

static int memory_doesFitBefore(const MemoryArea* area, const MemoryArea* check) {
    assert( check != NULL );
    if (area == NULL) {
        return 0;
    }
    if (check->end > area->start) {
        return -1;
    }
    return 0;
}

//int memory_doesFitAfter(const MemoryArea* area, const MemoryArea* check) {
//    assert( check != NULL );
//    if (area == NULL) {
//        return 0;
//    }
//    const size_t addrAfterEnd = area->offset + area->size;
//    if (check->offset < addrAfterEnd) {
//        return -1;
//    }
//    return 0;
//}

int memory_fitBetween(const MemoryArea* first, const MemoryArea* second, MemoryArea* check) {
    if (check==NULL) {
        return -1;
    }
    if (memory_doesFitBefore(second, check) != 0) {
        return -1;
    }

    if (first == NULL) {
        /// fits without changing
        return 0;
    }
    if (first->end <= check->start) {
        /// fits without changing
        return 0;
    }

    /// check space between
    if (second != NULL) {
        const size_t spaceBetween = second->start - first->end;
        const size_t checkSize = memory_size( check );
        if (spaceBetween < checkSize) {
            /// no enough free space
            return -1;
        }
    }
    /// there is space
    const size_t diff = first->end - check->start;
    check->start += diff;
    check->end += diff;
    return 0;
}

void memory_fitAfter(const MemoryArea* segment, MemoryArea* check) {
	assert( check != NULL );

	if ( segment == NULL ) {
		return ;
	}
    if (segment->end <= check->start) {
        return;
    }
    const size_t diff = segment->end - check->start;
    check->start += diff;
    check->end += diff;
}
