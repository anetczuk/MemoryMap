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


void memory_init( MemoryArea* area ) {
    //TODO: implement
}

void memory_release( MemoryArea* area ) {
    //TODO: implement
}

int memory_doesFitBefore(const MemoryArea* area, const MemoryArea* check) {
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
        const size_t checkSize = check->end - check->start;
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

int memory_fitAfter(const MemoryArea* segment, MemoryArea* check) {
    if (check==NULL) {
        return -1;
    }
    if (segment==NULL) {
        return 0;
    }
    if (segment->end <= check->start) {
        return 0;
    }
    const size_t diff = segment->end - check->start;
    check->start += diff;
    check->end += diff;
    return 0;
}
