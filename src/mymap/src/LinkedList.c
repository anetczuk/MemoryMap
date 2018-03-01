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

#include "mymap/LinkedList.h"

#include <stddef.h>                 /// NULL
#include <stdlib.h>                 /// free


void* list_mmap(LinkedList* list, void *vaddr, unsigned int size, unsigned int flags, void *o) {
    //TODO: implement
    return NULL;
}

void list_munmap(LinkedList* list, void *vaddr) {
    //TODO: implement
}

int list_init(LinkedList* list) {
    if (list == NULL) {
        return -1;
    }
    list->area.val = 0;
    list->next = NULL;
    return 0;
}


/// ===================================================


int list_add(LinkedList* list, int val) {
    if (list == NULL) {
        return -1;
    }

    /// finding last element
    LinkedList* curr = list;
    while( curr->next != NULL ) {
        curr = curr->next;
    }

    /// curr points to last element
    curr->next = calloc( 1, sizeof(LinkedList) );
    curr->area.val = val;

    return 0;
}

int list_release(LinkedList* list) {
    if (list == NULL) {
        return -1;
    }
    /**
     * Done in recursive manner. In case of very large lists consider
     * reimplementing it using while() and vector structure.
     */
    const int released = list_release(list->next);
    return released+1;
}
