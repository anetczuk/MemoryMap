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

#include <stdlib.h>                 /// free

#include "mymap/MemoryArea.h"


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

int list_getValue(LinkedList* list, const size_t index) {
    LinkedListItem* curr = list->root;
    for(size_t i=0; i<index; ++i) {
        curr = curr->next;
    }
    return curr->area.offset;
}

void list_insertNode(LinkedListItem** node) {
    LinkedListItem* old = *node;
    (*node) = calloc( 1, sizeof(LinkedListItem) );
    (*node)->next = old;
}

void list_insertValue(LinkedListItem** node, const size_t val) {
    list_insertNode(node);
    (*node)->area.offset = val;
}

/**
 * Here list element is always not NULL and it's value is smaller than 'val'.
 */
void list_addToNode(LinkedListItem* list, const size_t val) {
    LinkedListItem* curr = list;
    while( curr->next != NULL ) {
        if (curr->next->area.offset >= val) {
            list_insertValue(&(curr->next), val);
            return ;
        }
        curr = curr->next;
    }

    /// curr points to last element
    list_insertValue( &(curr->next), val );
    return ;
}

int list_add(LinkedList* list, const size_t val) {
    if (list == NULL) {
        return -1;
    }

    if (list->root == NULL) {
        list_insertValue(&(list->root), val);
        return 0;
    }

    if (list->root->area.offset >= val) {
        list_insertValue(&(list->root), val);
        return 0;
    }

    list_addToNode( list->root, val );
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


void* list_mmap(LinkedList* list, void *vaddr, unsigned int size, unsigned int flags, void *o) {
//    if (list == NULL) {
//        return NULL;
//    }
//    MemoryArea area;
//    area.offset = vaddr;
//    area.size = size;
//    area.flags = flags;
//    if (list->root == NULL) {
//        /// empty list
//        list_insertNode(&(list->root));
//        list->root->area = area;
//        return list->root->area.offset;
//    }
    return NULL;
}

void list_munmap(LinkedList* list, void *vaddr) {
    //TODO: implement
}

int list_init(LinkedList* list) {
    if (list == NULL) {
        return -1;
    }
    list->root = NULL;
    return 0;
}
