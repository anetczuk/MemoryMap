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

#include "rbtree/UIntRBTree.h"

#include <stdlib.h>                     /// malloc, free

#include "rbtree/AbstractRBTree.h"



static inline bool uirbtree_checkOrder(const ARBTreeValue valueA, const ARBTreeValue valueB) {
    const UIntRBTreeValue vA = *((UIntRBTreeValue*)valueA);
    const UIntRBTreeValue vB = *((UIntRBTreeValue*)valueB);
    return (vA < vB);
}

static inline bool uirbtree_canInsertRight(const ARBTreeValue valueA, const ARBTreeValue valueB) {
    return ( uirbtree_checkOrder(valueA, valueB) == false );
}

static inline bool uirbtree_canInsertLeft(const ARBTreeValue valueA, const ARBTreeValue valueB) {
    return ( uirbtree_checkOrder(valueA, valueB) == true );
}

static inline void uirbtree_printValue(const ARBTreeValue value) {
    const UIntRBTreeValue v = *((UIntRBTreeValue*)value);
    printf("%zu", v);
}

static inline void uirbtree_freeValue(ARBTreeValue value) {
    free(value);
}


/// ========================================================================================


bool uirbtree_init(UIntRBTree* tree) {
    if (tree == NULL) {
        return false;
    }
    ARBTree* baseTree = &(tree->tree);
    rbtree_init(baseTree);

    baseTree->fIsLessOrder = uirbtree_checkOrder;
    baseTree->fCanInsertRight = uirbtree_canInsertRight;
    baseTree->fCanInsertLeft = uirbtree_canInsertLeft;

    baseTree->fPrintValue = uirbtree_printValue;
    baseTree->fDeleteValue = uirbtree_freeValue;

    return true;
}

size_t uirbtree_size(const UIntRBTree* tree) {
    if (tree == NULL) {
        return 0;
    }
    const ARBTree* baseTree = &(tree->tree);
    return rbtree_size(baseTree);
}

size_t uirbtree_depth(const UIntRBTree* tree) {
    if (tree == NULL) {
        return 0;
    }
    const ARBTree* baseTree = &(tree->tree);
    return rbtree_depth(baseTree);
}

ARBTreeValidationError uirbtree_isValid(const UIntRBTree* tree) {
    if (tree == NULL) {
        return ARBTREE_INVALID_OK;
    }
    const ARBTree* baseTree = &(tree->tree);
    return rbtree_isValid(baseTree);
}

void uirbtree_print(const UIntRBTree* tree) {
    if (tree == NULL) {
        printf("%s", "[NULL]");
        return ;
    }
    const ARBTree* baseTree = &(tree->tree);
    rbtree_print(baseTree);
}

bool uirbtree_add(UIntRBTree* tree, const UIntRBTreeValue value) {
    if (tree == NULL) {
        return false;
    }
    ARBTree* baseTree = &(tree->tree);

    UIntRBTreeValue* ptr = malloc( sizeof(UIntRBTreeValue) );
    *ptr = value;

    if (rbtree_add(baseTree, ptr)==true) {
        return true;
    }

    free(ptr);
    return false;
}

bool uirbtree_delete(UIntRBTree* tree, const UIntRBTreeValue value) {
    if (tree == NULL) {
        return false;
    }
    ARBTree* baseTree = &(tree->tree);

    const ARBTreeValue v = (ARBTreeValue)&value;
    return rbtree_delete(baseTree, v);
}

bool uirbtree_release(UIntRBTree* tree) {
    if (tree == NULL) {
        return false;
    }
    ARBTree* baseTree = &(tree->tree);
    return rbtree_release(baseTree);
}

