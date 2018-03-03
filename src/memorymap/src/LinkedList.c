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

#include "memorymap/LinkedList.h"

#include <stdlib.h>                 /// free
#include <assert.h>


typedef struct LinkedListNode {
    MemoryArea area;
    struct LinkedListNode* next;
} LinkedListItem;


/// ===================================================


size_t list_size(LinkedList* list) {
    if (list==NULL)
        return 0;
    if (list->root==NULL)
        return 0;

    size_t retSize = 1;
    /// finding last element
    LinkedListItem* curr = list->root;
    while( curr->next != NULL ) {
        curr = curr->next;
        ++retSize;
    }

    return retSize;
}

const MemoryArea* list_get(LinkedList* list, const size_t index) {
    assert( list != NULL );

    LinkedListItem* curr = list->root;
    for(size_t i=0; i<index; ++i) {
        curr = curr->next;
    }
    return &(curr->area);
}

void list_insertNode(LinkedListItem** node) {
    LinkedListItem* old = *node;
    (*node) = calloc( 1, sizeof(LinkedListItem) );
    (*node)->next = old;
}

/**
 * We assume list and area are not NULL.
 */
void* list_addMemory(LinkedList* list, MemoryArea* area) {
    assert( list != NULL );
    assert( area != NULL );

    if (list->root == NULL) {
        list_insertNode(&(list->root));
        list->root->area = *area;
        return (void*)list->root->area.offset;
    }

    /// compare with first element
    const int doesFit = memory_fitBetween(NULL, &(list->root->area), area);
    if (doesFit == 0) {
        list_insertNode(&(list->root));
        list->root->area = *area;
        return (void*)list->root->area.offset;
    }

    /// compare with middle elements
    LinkedListItem* curr = list->root;
    while( curr->next != NULL ) {
        const int doesFit = memory_fitBetween(&(curr->area), &(curr->next->area), area);
        if (doesFit == 0) {
            list_insertNode(&(curr->next));
            curr->next->area = *area;
            return (void*)curr->next->area.offset;
        }
        curr = curr->next;
    }

    /// curr points to last element
    memory_fitAfter(&(curr->area), area);

    list_insertNode( &(curr->next) );
    curr->next->area = *area;
    return (void*)(curr->next->area.offset);
}

int list_add(LinkedList* list, const size_t address, const size_t size) {
    if (list == NULL) {
        return -1;
    }

    MemoryArea area;
    area.offset = address;
    area.size = size;

    if (list_addMemory(list, &area) == NULL) {
        return -1;
    }
    return 0;
}

int list_releaseNodes(LinkedListItem* list) {
    if (list == NULL) {
        return -1;
    }
    /**
     * Done in recursive manner. In case of very large lists consider
     * reimplementing it using while() and vector structure.
     */
    const int released = list_releaseNodes(list->next);
    free(list);
    return released+1;
}

int list_release(LinkedList* list) {
    if (list == NULL) {
        return -1;
    }
    const int released = list_releaseNodes(list->root);
    return released+1;
}


/// ===================================================


void* list_mmap(LinkedList* list, void *vaddr, unsigned int size) {
    if (list == NULL) {
        return NULL;
    }

    MemoryArea area;
    area.offset = (size_t)vaddr;
    area.size = size;

    return list_addMemory(list, &area);
}

void list_munmap(LinkedList* list, void *vaddr) {
    //TODO: implement

    /// find pointed segment
}

int list_init(LinkedList* list) {
    if (list == NULL) {
        return -1;
    }
    list->root = NULL;
    return 0;
}
