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


typedef enum {
    ARBTREE_COLOR_BLACK = 0,
    ARBTREE_COLOR_RED
} ARBTreeNodeColor;


typedef int ARBTreeValue;


typedef struct ARBTreeElement {
    struct ARBTreeElement* parent;
    struct ARBTreeElement* left;
    struct ARBTreeElement* right;
    ARBTreeValue value;
    ARBTreeNodeColor color;                			/// black by default
} ARBTreeNode;


typedef struct {
    ARBTreeNode* root;
} ARBTree;


typedef enum {
    ARBTREE_INVALID_OK = 0,                        /// tree is valid

    /// binary tree properties
    ARBTREE_INVALID_ROOT_PARENT = 1,
    ARBTREE_INVALID_NODE_PARENT = 2,
    ARBTREE_INVALID_SAME_CHILD = 3,                /// 'left' and 'right' points to the same node
    ARBTREE_INVALID_BAD_VALUE = 4,
    ARBTREE_INVALID_NOT_SORTED = 5,

    /// red-black tree properties
    ARBTREE_INVALID_RED_ROOT = 6,
    ARBTREE_INVALID_BLACK_CHILDREN = 7,            /// when node is red, then children have to be black
    ARBTREE_INVALID_BLACK_PATH = 8                 /// invalid number of black nodes on paths
} ARBTreeValidationError;


/// ===========================================================================


bool rbtree_checkOrder(const ARBTreeValue valueA, const ARBTreeValue valueB) {
	(void) valueA; /* unused */
	(void) valueB; /* unused */
	return (valueA < valueB);
}

bool rbtree_isValidValue(const ARBTreeValue value) {
	(void) value; /* unused */
	return true;                        /// always valid
}

bool rbtree_isValid_isOrder(const ARBTreeNode* node, const ARBTreeNode* nextNode) {
	(void) node;     /* unused */
	(void) nextNode; /* unused */
	return true;                        /// always valid
}

void rbtree_printValue(const ARBTreeValue value) {
	printf("%d", value);
}

/// ===========================================================================


/**
 * If list has be initialized previously, then have to be released
 * before next initialization.
 */
int rbtree_init(ARBTree* tree);

size_t rbtree_size(const ARBTree* tree);

size_t rbtree_depth(const ARBTree* tree);

ARBTreeValidationError rbtree_isValid(const ARBTree* tree);

ARBTreeNode* rbtree_findNode(const ARBTree* tree, const ARBTreeValue value);

bool rbtree_add(ARBTree* tree, const ARBTreeValue value);

bool rbtree_delete(ARBTree* tree, const ARBTreeValue value);

void rbtree_print(const ARBTree* tree);

/**
 * List has to be initialized before releasing.
 * Returns number of released elements (size of list)
 */
int rbtree_release(ARBTree* tree);


/// =================================================================


ARBTreeNode* rbtree_makeDefaultNode();

ARBTreeNode* rbtree_makeColoredNode(const ARBTreeNodeColor color);

size_t rbtree_nodeIndex(const ARBTreeNode* node);


#endif /* SRC_RBTREE_INCLUDE_RBTREE_ABSTRACTRBTREE_H_ */
