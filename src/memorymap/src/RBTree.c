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

#include "memorymap/RBTree.h"

#include "memorymap/MemoryArea.h"


typedef struct LinkedListNode {
    MemoryArea area;
    struct LinkedListNode* next;
} LinkedListItem;


/// ===================================================


size_t tree_size(RBTree* tree) {
    //TODO: implement
    return -1;
}

int tree_add(RBTree* tree, const size_t address, const size_t size) {
    //TODO: implement
    return -1;
}

int tree_release(RBTree* tree) {
    //TODO: implement
    return -1;
}


/// ===================================================


void* tree_mmap(RBTree* tree, void *vaddr, unsigned int size) {
    //TODO: implement
    return NULL;
}

void tree_munmap(RBTree* tree, void *vaddr) {
    //TODO: implement
}

int tree_init(RBTree* tree) {
    //TODO: implement
    return -1;
}
