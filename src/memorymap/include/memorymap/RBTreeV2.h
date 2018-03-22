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

#ifndef RBTREEV2_H_
#define RBTREEV2_H_

#include <stddef.h>                            /// NULL, size_t

#include "rbtree/AbstractRBTree.h"
#include "memorymap/MemoryArea.h"



typedef ARBTreeNode RBTreeNode2;
typedef ARBTree RBTree2;


/// ===========================================================================


void* tree2_mmap(RBTree2* tree, void *vaddr, unsigned int size);

void tree2_munmap(RBTree2* tree, void *vaddr);

/**
 * If list has be initialized previously, then have to be released
 * before next initialization.
 */
int tree2_init(RBTree2* tree);


/// =============================================


size_t tree2_size(const RBTree2* tree);

size_t tree2_depth(const RBTree2* tree);

size_t tree2_startAddress(const RBTree2* tree);

size_t tree2_endAddress(const RBTree2* tree);

MemoryArea tree2_area(const RBTree2* tree);

ARBTreeValidationError tree2_isValid(const RBTree2* tree);

size_t tree2_add(RBTree2* tree, const size_t address, const size_t size);

void tree2_delete(RBTree2* tree, const size_t address);

void tree2_print(const RBTree2* tree);

/**
 * List has to be initialized before releasing.
 * Returns number of released elements (size of list)
 */
int tree2_release(RBTree2* tree);


#endif /* RBTREEV2_H_ */
