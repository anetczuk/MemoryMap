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

#include "memorymap/RBTree.h"

#include <stdlib.h>                     /// free
#include <assert.h>

#include "memorymap/MemoryArea.h"


typedef enum {
    BLACK = 0,
    RED
} NodeColor;


typedef struct RBTreeNode {
    struct RBTreeNode* left;
    struct RBTreeNode* right;
    MemoryArea area;
    NodeColor color;                /// black by default
} RBTreeItem;


/// ===================================================


static size_t tree_sizeSubtree(RBTreeItem* tree) {
    if (tree==NULL) {
        return 0;
    }
    return tree_sizeSubtree(tree->left) + tree_sizeSubtree(tree->right) + 1;
}

size_t tree_size(RBTree* tree) {
    if (tree==NULL)
        return 0;
    return tree_sizeSubtree(tree->root);
}

//void* tree_addMemoryToLeft(RBTreeItem* node, MemoryArea* area) {
//    //TODO: impleent
//
//    if ( node->left == NULL ) {
//        /// compare with first element
//        const int doesFit = memory_fitBetween(NULL, &(node->area), area);
//        if (doesFit == 0) {
//            node->left = calloc( 1, sizeof(RBTreeItem) );
//            node->left->area = *area;
//            return (void*)node->left->area.start;
//        }
//        return NULL;
//    }
//
////    /// compare with first element
////    const int doesFit = memory_fitBetween(NULL, &(list->root->area), area);
////    if (doesFit == 0) {
////        list_insertNode(&(list->root));
////        list->root->area = *area;
////        return (void*)list->root->area.start;
////    }
////
////    /// compare with middle elements
////    LinkedListItem* curr = list->root;
////    while( curr->next != NULL ) {
////        const int doesFit = memory_fitBetween(&(curr->area), &(curr->next->area), area);
////        if (doesFit == 0) {
////            list_insertNode(&(curr->next));
////            curr->next->area = *area;
////            return (void*)curr->next->area.start;
////        }
////        curr = curr->next;
////    }
////
////    /// curr points to last element
////    memory_fitAfter(&(curr->area), area);
////
////    list_insertNode( &(curr->next) );
////    curr->next->area = *area;
////    return (void*)(curr->next->area.start);
//
//    return NULL;
//}
//
//void* tree_addMemoryToRight(RBTreeItem* node, MemoryArea* area) {
//    //TODO: impleent
//    return NULL;
//}

void* tree_addMemory(RBTree* tree, MemoryArea* area) {
    assert( tree != NULL );
    assert( area != NULL );

    if (tree->root == NULL) {
        tree->root = calloc( 1, sizeof(RBTreeItem) );
        tree->root->area = *area;
        return (void*)tree->root->area.start;
    }

    return NULL;

//    RBTreeItem* curr = tree->root;
//    if (area->start < curr->area.start) {
//        /// go to left
//        void* found = tree_addMemoryToLeft( curr, area );
//        if (found != NULL) {
//            return found;
//        }
//        /// can happen that could not find free block, then check right side
//    }
//    /// go to right
//    return tree_addMemoryToRight( curr, area );          /// always finds (at worst after last segment)
}

int tree_add(RBTree* tree, const size_t address, const size_t size) {
    if (tree == NULL) {
        return -1;
    }

    MemoryArea area = memory_create(address, size);

    if (tree_addMemory(tree, &area) == NULL) {
        return -1;
    }
    return 0;
}

int tree_releaseNodes(RBTreeItem* tree) {
    if (tree == NULL) {
        return -1;
    }
    /**
     * Done in recursive manner. In case of very large structures consider
     * reimplementing it using while() and vector structure.
     */
    const int leftReleased = tree_releaseNodes(tree->left);
    const int rightReleased = tree_releaseNodes(tree->right);
    free(tree);
    return leftReleased+rightReleased+1;
}

int tree_release(RBTree* tree) {
    if (tree==NULL) {
        return -1;
    }
    return tree_releaseNodes(tree->root)+1;
}


/// ===================================================


void* tree_mmap(RBTree* tree, void *vaddr, unsigned int size) {
    //TODO: implement
    return NULL;
}

void tree_munmap(RBTree* tree, void *vaddr) {
    //TODO: implement
}

int tree_init(RBTree* tree) {
    if (tree == NULL) {
        return -1;
    }

    tree->root = NULL;
    return 0;
}
