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
#include "memorymap/RBTreeV2.h"

#include <time.h>
#include <stdlib.h>
#include <assert.h>
//#include <stdio.h>                              /// printf



static unsigned int current_seed = 0;

static unsigned int get_next_seed() {
    if (current_seed == 0) {
        srand( time(NULL) );
        current_seed = rand();
    }
    return (++current_seed);
}

static void test_trees_comparison() {
    const unsigned int seed = get_next_seed();
//    const unsigned int seed = 1753553511;
    srand( seed );

//    printf("seed: %u\n", seed);

    static const size_t nodes_num = 100;
    static const size_t max_address = 2000;
    static const size_t max_size = 50;

    RBTree tree;
    tree_init(&tree);

    RBTree2 tree2;
    tree2_init(&tree2);


    /// adding random elements

    for(size_t i = 0; i < nodes_num; ++i) {
        const size_t addr = rand() % max_address +1;
        const size_t msize = rand() % max_size +1;

        tree_add(&tree, addr, msize);
        tree2_add(&tree2, addr, msize);
    }

    const MemoryArea area1 = tree_area(&tree);
    const MemoryArea area2 = tree2_area(&tree2);
    assert( memory_isEqual( &area1, &area2 ) == true );


    /// deleting random elements

    for(size_t i = 0; i < nodes_num; ++i) {
        const size_t addr = rand() % memory_size(&area1) + area1.start;

        /// printf("Iteration %lu: removing %lu\n", i, addr);

        tree_delete(&tree, addr);
        tree2_delete(&tree2, addr);
    }

    ///printf("Releasing\n");
    tree_release(&tree);
    tree2_release(&tree2);
}


/// ==================================================


int main(void) {

    test_trees_comparison();

    return 0;
}
