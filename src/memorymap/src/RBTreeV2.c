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

#include "memorymap/RBTreeV2.h"

#include <stdlib.h>                     /// free
#include <assert.h>
#include <stdio.h>                      /// printf
#include <string.h>



static const RBTreeNode2* tree2_getLeftmostNode(const RBTreeNode2* node) {
    if (node == NULL) {
        return NULL;
    }
    const RBTreeNode2* curr = node;
    while (curr->left != NULL) {
        curr = curr->left;
    }
    return curr;
}

static const RBTreeNode2* tree2_getRightmostNode(const RBTreeNode2* node) {
    if (node == NULL) {
        return NULL;
    }
    const RBTreeNode2* curr = node;
    while (curr->right != NULL) {
        curr = curr->right;
    }
    return curr;
}

///**
// * Returns ancestor on left side of current node.
// */
//static const RBTreeNode2* tree2_getLeftAncestor(const RBTreeNode2* node) {
//    const RBTreeNode2* child = node;
//    const RBTreeNode2* curr = node->parent;
//    while( curr != NULL ) {
//        if (curr->right == child) {
//            return curr;
//        }
//        child = curr;
//        curr = curr->parent;
//    }
//    /// root found
//    return NULL;
//}


/// ===================================================


size_t tree2_size(const RBTree2* tree) {
    (void) tree; /* unused */
    //TODO: implement
    return 0;
}

size_t tree2_depth(const RBTree2* tree) {
    (void) tree; /* unused */
    //TODO: implement
    return 0;
}

size_t tree2_startAddress(const RBTree2* tree) {
    if (tree==NULL)
        return 0;
    if (tree->root==NULL)
        return 0;

    const RBTreeNode2* node = tree2_getLeftmostNode(tree->root);
    const MemoryArea* area = (const MemoryArea*)node->value;
    return area->start;
}

size_t tree2_endAddress(const RBTree2* tree) {
    if (tree==NULL)
        return 0;
    if (tree->root==NULL)
        return 0;

    const RBTreeNode2* node = tree2_getRightmostNode(tree->root);
    const MemoryArea* area = (const MemoryArea*)node->value;
    return area->end;
}

MemoryArea tree2_area(const RBTree2* tree) {
    if (tree==NULL)
        return memory_create(0, 0);
    if (tree->root==NULL)
        return memory_create(0, 0);

    const size_t startAddress = tree2_startAddress(tree);
    const size_t endAddress = tree2_endAddress(tree);
    const size_t addressSpace = endAddress - startAddress;
    return memory_create(startAddress, addressSpace);
}


/// ==================================================================================


ARBTreeValidationError tree2_isValid(const RBTree2* tree) {
    (void) tree; /* unused */
    //TODO: implement
    return -1;
}


/// ==================================================================================


size_t tree2_add(RBTree2* tree, const size_t address, const size_t size) {
    (void) tree; /* unused */
    (void) address; /* unused */
    (void) size; /* unused */
//    if (tree == NULL) {
//        return 0;
//    }
//    MemoryArea area = memory_create(address, size);
//    void* retAddr = tree2_addMemory(tree, &area);
//    /// assert( tree2_isValid(tree) == 0 );
//    return (size_t)retAddr;
    return 0;
}


/// ==============================================================================================


void tree2_print(const RBTree2* tree) {
    if (tree == NULL) {
        printf("%s", "[NULL]");
        return ;
    }
//    tree2_printWhole(tree->root);
    //TODO: implement
}


/// ==============================================================================================


static int tree2_releaseNodes(RBTreeNode2* node) {
    if (node == NULL) {
        return 0;
    }
    /**
     * Done in recursive manner. In case of very large structures consider
     * reimplementing it using while() and vector structure.
     */
    const int leftReleased = tree2_releaseNodes(node->left);
    const int rightReleased = tree2_releaseNodes(node->right);
    free(node);
    return leftReleased+rightReleased+1;
}

int tree2_release(RBTree2* tree) {
    if (tree==NULL) {
        return -1;
    }
    const int ret = tree2_releaseNodes(tree->root);
    tree->root = NULL;
    return ret;
}


/// ===================================================


void* tree2_mmap(RBTree2* tree, void *vaddr, unsigned int size) {
    (void) vaddr; /* unused */
    (void) size; /* unused */

    if (tree == NULL) {
        return NULL;
    }

//    MemoryArea area = memory_create( (size_t)vaddr, size );
//    void* retAddr = tree2_addMemory(tree, &area);
//    assert( tree2_isValid(tree) == ARBTREE_INVALID_OK );
//    return retAddr;

    //TODO: implement

    return NULL;
}


/// =========================================================================


void tree2_delete(RBTree2* tree, const size_t address) {
    (void) tree; /* unused */
    (void) address; /* unused */
    //TODO: implement
}


/// =========================================================================


void tree2_munmap(RBTree2* tree, void *vaddr) {
    if (tree == NULL) {
        return ;
    }
    if (tree->root==NULL) {
        return ;
    }

    const size_t voffset = (size_t)vaddr;
    tree2_delete(tree, voffset);
    assert( tree2_isValid(tree) == ARBTREE_INVALID_OK );
}

int tree2_init(RBTree2* tree) {
    if (tree == NULL) {
        return -1;
    }

    tree->root = NULL;
    return 0;
}

