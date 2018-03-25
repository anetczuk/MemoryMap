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

#include "rbtree/AbstractRBTree.h"


typedef ARBTreeNode RBTreeNode2;


static inline bool tree2_checkOrder(const ARBTreeValue valueA, const ARBTreeValue valueB) {
    const MemoryArea* vA = (MemoryArea*)valueA;
    const MemoryArea* vB = (MemoryArea*)valueB;
    const int ret = memory_compare(vA, vB);
    return (ret < 0);
}

static inline bool tree2_canInsertRight(const ARBTreeValue valueA, const ARBTreeValue valueB) {
    (void) valueA; /* unused */
    (void) valueB; /* unused */
    /// always can add to right
    return true;
}

static inline void tree2_printValue(const ARBTreeValue value) {
    const MemoryArea* v = (MemoryArea*)value;
    ///memory_print(v);
    printf("%03lx,%02lx", v->start, v->end);
}

static inline void tree2_freeValue(ARBTreeValue value) {
    free(value);
}

static inline bool tree2_tryFitRight(const ARBTreeNode* node, ARBTreeValue value) {
    const ARBTreeNode* ancestor = rbtree_getRightAncestor(node);
    if (ancestor == NULL) {
        /// greatest leaf case -- add node
        memory_fitAfter(node->value, value);
        return true;
    }

    /// check space between nodes
    const int doesFit = memory_fitBetween(node->value, ancestor->value, value);
    return (doesFit == 0);
}

static inline bool tree2_tryFitLeft(const ARBTreeNode* node, ARBTreeValue value) {
    const ARBTreeNode* ancestor = rbtree_getLeftAncestor(node);
    if (ancestor == NULL) {
        /// smallest leaf case -- add node
        return true;
    }

    /// check space between nodes
    const int doesFit = memory_fitBetween(ancestor->value, node->value, value);
    return (doesFit == 0);
}


/// ========================================================================================


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


/// ===================================================


size_t tree2_size(const RBTree2* tree) {
    if (tree == NULL) {
        return 0;
    }
    const ARBTree* baseTree = &(tree->tree);
    return rbtree_size(baseTree);
}

size_t tree2_depth(const RBTree2* tree) {
    if (tree == NULL) {
        return 0;
    }
    const ARBTree* baseTree = &(tree->tree);
    return rbtree_depth(baseTree);
}

size_t tree2_startAddress(const RBTree2* tree) {
    if (tree==NULL)
        return 0;
    const ARBTree* baseTree = &(tree->tree);
    if (baseTree->root==NULL)
        return 0;

    const RBTreeNode2* node = tree2_getLeftmostNode(baseTree->root);
    const MemoryArea* area = (const MemoryArea*)node->value;
    return area->start;
}

size_t tree2_endAddress(const RBTree2* tree) {
    if (tree==NULL)
        return 0;
    const ARBTree* baseTree = &(tree->tree);
    if (baseTree->root==NULL)
        return 0;

    const RBTreeNode2* node = tree2_getRightmostNode(baseTree->root);
    const MemoryArea* area = (const MemoryArea*)node->value;
    return area->end;
}

MemoryArea tree2_area(const RBTree2* tree) {
    if (tree==NULL)
        return memory_create(0, 0);

    const size_t startAddress = tree2_startAddress(tree);
    const size_t endAddress = tree2_endAddress(tree);
    const size_t addressSpace = endAddress - startAddress;
    return memory_create(startAddress, addressSpace);
}

MemoryArea tree2_valueByIndex(const RBTree2* tree, const size_t index) {
    if (tree == NULL) {
        return memory_create(0, 0);
    }
    const ARBTree* baseTree = &(tree->tree);
    const MemoryArea* val = (const MemoryArea*)rbtree_valueByIndex(baseTree, index);
    if (val==NULL) {
        return memory_create(0, 0);
    }
    return *val;
}


/// ==================================================================================


ARBTreeValidationError tree2_isValid(const RBTree2* tree) {
    if (tree == NULL) {
        return ARBTREE_INVALID_OK;
    }
    const ARBTree* baseTree = &(tree->tree);
    return rbtree_isValid(baseTree);
}


/// ==================================================================================


size_t tree2_add(RBTree2* tree, const size_t address, const size_t size) {
    if (tree==NULL)
        return 0;
    ARBTree* baseTree = &(tree->tree);
    if (baseTree==NULL)
        return 0;

    MemoryArea* ptr = malloc( sizeof(MemoryArea) );
    *ptr = memory_create(address, size);

    if (rbtree_add(baseTree, ptr)==true) {
        return ptr->start;
    }

    free(ptr);
    return 0;
}

void tree2_delete(RBTree2* tree, const size_t address) {
    if (tree == NULL) {
        return ;
    }
    ARBTree* baseTree = &(tree->tree);

    MemoryArea area = memory_create(address, 1);
    const ARBTreeValue v = (ARBTreeValue)&area;
    rbtree_delete(baseTree, v);
}


/// ==============================================================================================


void tree2_print(const RBTree2* tree) {
    if (tree == NULL) {
        printf("%s", "[NULL]");
        return ;
    }
    const ARBTree* baseTree = &(tree->tree);
    rbtree_print(baseTree);
}


/// ==============================================================================================


bool tree2_release(RBTree2* tree) {
    if (tree == NULL) {
        return false;
    }
    ARBTree* baseTree = &(tree->tree);
    return rbtree_release(baseTree);
}


/// ===================================================


void* tree2_mmap(RBTree2* tree, void *vaddr, unsigned int size) {
    if (tree==NULL)
        return NULL;
    ARBTree* baseTree = &(tree->tree);
    if (baseTree==NULL)
        return NULL;

    MemoryArea* ptr = malloc( sizeof(MemoryArea) );
    *ptr = memory_create((size_t)vaddr, size);

    if (rbtree_add(baseTree, ptr)==true) {
        return (void*)ptr->start;
    }

    free(ptr);
    return NULL;
}

void tree2_munmap(RBTree2* tree, void *vaddr) {
    const size_t voffset = (size_t)vaddr;
    tree2_delete(tree, voffset);
    assert( tree2_isValid(tree) == ARBTREE_INVALID_OK );
}

bool tree2_init(RBTree2* tree) {
    if (tree == NULL) {
        return false;
    }

    ARBTree* baseTree = &(tree->tree);
    rbtree_init(baseTree);

    baseTree->fIsLessOrder = tree2_checkOrder;
    baseTree->fCanInsertRight = tree2_canInsertRight;
    baseTree->fCanInsertLeft = tree2_checkOrder;

    baseTree->fTryFitRight = tree2_tryFitRight;
    baseTree->fTryFitLeft = tree2_tryFitLeft;

    baseTree->fPrintValue = tree2_printValue;
    baseTree->fDeleteValue = tree2_freeValue;

    return true;
}

