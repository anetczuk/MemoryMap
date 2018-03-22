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

#ifndef SRC_RBTREE_INCLUDE_RBTREE_ABSTRACTRBTREE_H_
#define SRC_RBTREE_INCLUDE_RBTREE_ABSTRACTRBTREE_H_

#include <stddef.h>                            /// NULL, size_t
#include <stdio.h>                      	   /// printf
#include <stdbool.h>

#include "rbtree/AbstractRBTreeDefs.h"



typedef struct ARBTreeElement {
    struct ARBTreeElement* parent;
    struct ARBTreeElement* left;
    struct ARBTreeElement* right;
    ARBTreeValue value;
    ARBTreeNodeColor color;                			/// black by default
} ARBTreeNode;


/// ===========================================================================


/**
 * If list has be initialized previously, then have to be released
 * before next initialization.
 */
bool rbtree_init(ARBTree* tree);

size_t rbtree_size(const ARBTree* tree);

size_t rbtree_depth(const ARBTree* tree);

ARBTreeValidationError rbtree_isValid(const ARBTree* tree);

void rbtree_print(const ARBTree* tree);

/**
 * List has to be initialized before releasing.
 * Returns true if released, otherwise false.
 */
bool rbtree_release(ARBTree* tree);


/// =================================================================


ARBTreeNode* rbtree_findNode(const ARBTree* tree, const ARBTreeValue value);

bool rbtree_add(ARBTree* tree, const ARBTreeValue value);

bool rbtree_delete(ARBTree* tree, const ARBTreeValue value);


/// =================================================================


ARBTreeNode* rbtree_makeDefaultNode();

ARBTreeNode* rbtree_makeColoredNode(const ARBTreeNodeColor color);

size_t rbtree_nodeIndex(const ARBTreeNode* node);


#endif /* SRC_RBTREE_INCLUDE_RBTREE_ABSTRACTRBTREE_H_ */
