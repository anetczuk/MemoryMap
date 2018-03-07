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
    RBTREE_BLACK = 0,
    RBTREE_RED
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


/// =============================================


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

int tree_isValid(const RBTree* tree);

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


RBTreeNode* node_makeDefault();

RBTreeNode* node_makeColored(const NodeColor color);

RBTreeNode* node_makeLeaf(const NodeColor color, MemoryArea area);

RBTreeNode* node_makeFull(RBTreeNode* left, RBTreeNode* right, MemoryArea area, const NodeColor color);

void node_init(RBTreeNode* node);

void node_release(RBTreeNode* node);

size_t node_index(const RBTreeNode* node);

void node_setArea(RBTreeNode* node, const MemoryArea* area);

void node_setColor(RBTreeNode* node, const NodeColor color);

RBTreeNode* node_getLeft(RBTreeNode* node);

RBTreeNode* node_getRight(RBTreeNode* node);

void node_connectLeft(RBTreeNode* node, RBTreeNode* child);

void node_connectRight(RBTreeNode* node, RBTreeNode* child);


#endif /* RBTREE_H_ */
