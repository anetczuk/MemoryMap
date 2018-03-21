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


static inline bool uirbtree_isValidValue(const ARBTreeValue value) {
    return (value != NULL);
}

static inline bool uirbtree_checkOrder(const ARBTreeValue valueA, const ARBTreeValue valueB) {
    const UIntRBTreeValue vA = *((UIntRBTreeValue*)valueA);
    const UIntRBTreeValue vB = *((UIntRBTreeValue*)valueB);
    return (vA < vB);
}

static inline void uirbtree_printValue(const ARBTreeValue value) {
    const UIntRBTreeValue v = *((UIntRBTreeValue*)value);
    printf("%lu", v);
}


/// ========================================================================================


int uirbtree_init(ARBTree* tree) {
    const int ret = rbtree_init(tree);
    if (ret!=0) {
        return ret;
    }

    tree->fIsValidValue = uirbtree_isValidValue;
    tree->fCheckOrder = uirbtree_checkOrder;
    tree->fPrintValue = uirbtree_printValue;

    return ret;
}

bool uirbtree_add(ARBTree* tree, const UIntRBTreeValue value) {
    UIntRBTreeValue* ptr = malloc( sizeof(UIntRBTreeValue) );
    *ptr = value;

    if (rbtree_add(tree, ptr)==true) {
        return true;
    }

    free(ptr);
    return false;
}

bool uirbtree_delete(ARBTree* tree, const UIntRBTreeValue value) {
    const ARBTreeValue v = (ARBTreeValue)&value;
    return rbtree_delete(tree, v);
}
