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

#ifndef RBTREE_H_
#define RBTREE_H_

#include <stddef.h>                            /// NULL, size_t

#include "memorymap/MemoryArea.h"



typedef enum {
    RBTREE_COLOR_BLACK = 0,
    RBTREE_COLOR_RED
} NodeColor;


typedef struct RBTreeElement {
    struct RBTreeElement* parent;
    struct RBTreeElement* left;
    struct RBTreeElement* right;
    MemoryArea area;
    NodeColor color;                /// black by default
} RBTreeNode;


typedef struct {
    RBTreeNode* root;
} RBTree;


typedef enum {
    RBTREE_INVALID_OK = 0,                        /// tree is valid

    /// binary tree properties
    RBTREE_INVALID_ROOT_PARENT = 1,
    RBTREE_INVALID_NODE_PARENT = 2,
    RBTREE_INVALID_SAME_CHILD = 3,                /// 'left' and 'right' points to the same node
    RBTREE_INVALID_BAD_MEMORY_SEGMENT = 4,
    RBTREE_INVALID_NOT_SORTED = 5,

    /// red-black tree properties
    RBTREE_INVALID_RED_ROOT = 6,
    RBTREE_INVALID_BLACK_CHILDREN = 7,            /// when node is red, then children have to be black
    RBTREE_INVALID_BLACK_PATH = 8                 /// invalid number of black nodes on paths
} RBTreeValidationError;


/// ===========================================================================


void* tree_mmap(RBTree* tree, void *vaddr, unsigned int size);

void tree_munmap(RBTree* tree, void *vaddr);

/**
 * If list has be initialized previously, then have to be released
 * before next initialization.
 */
int tree_init(RBTree* tree);


/// =============================================


size_t tree_size(const RBTree* tree);

size_t tree_depth(const RBTree* tree);

size_t tree_startAddress(const RBTree* tree);

size_t tree_endAddress(const RBTree* tree);

MemoryArea tree_area(const RBTree* tree);

RBTreeValidationError tree_isValid(const RBTree* tree);

RBTreeNode* tree_findNode(const RBTree* tree, const size_t adress);

size_t tree_add(RBTree* tree, const size_t address, const size_t size);

void tree_delete(RBTree* tree, const size_t address);

void tree_print(const RBTree* tree);

/**
 * List has to be initialized before releasing.
 * Returns number of released elements (size of list)
 */
int tree_release(RBTree* tree);


/// =================================================================


RBTreeNode* tree_makeDefaultNode();

RBTreeNode* tree_makeColoredNode(const NodeColor color);

size_t tree_nodeIndex(const RBTreeNode* node);



#endif /* RBTREE_H_ */
