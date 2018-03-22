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

#ifndef SRC_RBTREE_INCLUDE_RBTREE_ABSTRACTRBTREEDEFS_H_
#define SRC_RBTREE_INCLUDE_RBTREE_ABSTRACTRBTREEDEFS_H_

#include <stdbool.h>


typedef enum {
    ARBTREE_COLOR_BLACK = 0,
    ARBTREE_COLOR_RED
} ARBTreeNodeColor;


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


struct ARBTreeElement;


/// ==================================================================


typedef void* ARBTreeValue;


typedef bool (* rbtree_isValidValue)(const ARBTreeValue value);

typedef bool (* rbtree_isLessOrder)(const ARBTreeValue valueA, const ARBTreeValue valueB);

typedef void (* rbtree_printValue)(const ARBTreeValue value);

typedef void (* rbtree_freeValue)(ARBTreeValue value);

typedef bool (* rbtree_canInsert)(const struct ARBTreeElement* node, ARBTreeValue value);


/// ==================================================================


struct ARBTreeElement;


typedef struct {
    struct ARBTreeElement* root;

    rbtree_isValidValue fIsValidValue;
    rbtree_isLessOrder fIsLessOrder;
    rbtree_printValue fPrintValue;
    rbtree_freeValue fFreeValue;            /// destroy value (release memory etc)

    rbtree_canInsert fCanInsertRight;
    rbtree_canInsert fCanInsertLeft;
} ARBTree;


#endif /* SRC_RBTREE_INCLUDE_RBTREE_ABSTRACTRBTREEDEFS_H_ */
