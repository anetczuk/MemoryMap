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

#include <time.h>
#include <stdlib.h>
#include <stdio.h>                              /// printf

/// for cmocka to mock system functions
#define UNIT_TESTING 1

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>


static void tree_mmap_NULL(void **state) {
    (void) state; /* unused */

    const void* ret = tree_mmap(NULL, NULL, 0);

    assert_null( ret );
}

static void tree_mmap_first(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    const void* ret = tree_mmap(&tree, (void*)128, 64);
    assert_int_equal( ret, 128 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_mmap_second(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)160, 64);

    const void* ret = tree_mmap(&tree, (void*)128, 64);
    assert_int_equal( ret, 224 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_mmap_segmented_toLeft(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)200, 64);
    /// small space between segments
    tree_mmap(&tree, (void*)100, 64);

    const void* ret = tree_mmap(&tree, (void*)128, 64);
    assert_int_equal( ret, 264 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_mmap_segmented_toRight(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)100, 64);
    /// small space between segments
    tree_mmap(&tree, (void*)200, 64);

    const void* ret = tree_mmap(&tree, (void*)128, 64);
    assert_int_equal( ret, 264 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_munmap_NULL(void **state) {
    (void) state; /* unused */

    tree_munmap(NULL, NULL);

    assert_true( 1 );
}

static void tree_munmap_empty(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_munmap(&tree, NULL);

    tree_release(&tree);
}

static void tree_munmap_badaddr(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);
    tree_mmap(&tree, (void*)100, 64);

    tree_munmap(&tree, (void*)50);

    const size_t ret = tree_size(&tree);
    assert_int_equal( ret, 1 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_munmap_root(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)100, 64);
    tree_mmap(&tree, (void*)200, 64);

    tree_munmap(&tree, (void*)120);

    const size_t ret = tree_size(&tree);
    assert_int_equal( ret, 1 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_munmap_root2(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)100, 64);
    tree_mmap(&tree, (void*)20, 64);
    tree_mmap(&tree, (void*)200, 64);

    tree_munmap(&tree, (void*)120);

    const size_t ret = tree_size(&tree);
    assert_int_equal( ret, 2 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_munmap_right(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)100, 64);
    tree_mmap(&tree, (void*)200, 64);

    tree_munmap(&tree, (void*)220);

    const size_t ret = tree_size(&tree);
    assert_int_equal( ret, 1 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_munmap_right2(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)100, 64);
    tree_mmap(&tree, (void*)20, 64);
    tree_mmap(&tree, (void*)200, 64);

    tree_munmap(&tree, (void*)220);

    const size_t ret = tree_size(&tree);
    assert_int_equal( ret, 2 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_munmap_left(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)200, 64);
    tree_mmap(&tree, (void*)100, 64);
    tree_mmap(&tree, (void*)300, 64);

    tree_munmap(&tree, (void*)120);

    const size_t ret = tree_size(&tree);
    assert_int_equal( ret, 2 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_munmap_left2(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)200, 64);
    tree_mmap(&tree, (void*)100, 64);
    tree_mmap(&tree, (void*)300, 64);
    tree_mmap(&tree, (void*)400, 64);

    tree_munmap(&tree, (void*)320);

    const size_t ret = tree_size(&tree);
    assert_int_equal( ret, 2 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_init_NULL(void **state) {
    (void) state; /* unused */

    const int ret = tree_init(NULL);
    assert_int_equal( ret, -1 );
}

static void tree_init_valid(void **state) {
    (void) state; /* unused */

    RBTree tree;
    const int ret = tree_init(&tree);
    assert_int_equal( ret, 0 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}


/// ==================================================


static void tree_add_NULL(void **state) {
    (void) state; /* unused */

    const size_t ret = tree_add(NULL, 3, 1);
    assert_int_equal( ret, 0 );
}

static void tree_add_left(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_add(&tree, 3, 1);
    tree_add(&tree, 1, 1);

    const size_t lSize = tree_size(&tree);
    assert_int_equal( lSize, 2 );

    const size_t depth = tree_depth(&tree);
    assert_int_equal( depth, 2 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_add_right(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_add(&tree, 3, 1);
    tree_add(&tree, 6, 1);

    const size_t lSize = tree_size(&tree);
    assert_int_equal( lSize, 2 );

    const size_t depth = tree_depth(&tree);
    assert_int_equal( depth, 2 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_add_subtree_left(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_add(&tree, 50, 1);
    tree_add(&tree, 20, 1);
    tree_add(&tree, 30, 1);
    tree_add(&tree, 40, 1);

    const size_t lSize = tree_size(&tree);
    assert_int_equal( lSize, 4 );

    const size_t depth = tree_depth(&tree);
    assert_int_equal( depth, 3 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_add_subtree_right(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_add(&tree, 50, 1);
    tree_add(&tree, 80, 1);
    tree_add(&tree, 70, 1);
    tree_add(&tree, 60, 1);

    const size_t lSize = tree_size(&tree);
    assert_int_equal( lSize, 4 );

    const size_t depth = tree_depth(&tree);
    assert_int_equal( depth, 3 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_add_subtree(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_add(&tree, 13, 1);
    tree_add(&tree, 8, 1);
    tree_add(&tree, 1, 1);
    tree_add(&tree, 6, 1);
    tree_add(&tree, 11, 1);

    tree_add(&tree, 17, 1);
    tree_add(&tree, 15, 1);
    tree_add(&tree, 25, 1);
    tree_add(&tree, 22, 1);
    tree_add(&tree, 27, 1);

    const size_t lSize = tree_size(&tree);
    assert_int_equal( lSize, 10 );

    const size_t depth = tree_depth(&tree);
    assert_int_equal( depth, 4 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_add_subtree_space(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_add(&tree, 50, 10);
    tree_add(&tree, 30, 10);
    tree_add(&tree, 35, 15);

    const size_t lSize = tree_size(&tree);
    assert_int_equal( lSize, 3 );

    const size_t depth = tree_depth(&tree);
    assert_int_equal( depth, 2 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_add_subtree_startAddr(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_add(&tree, 1383, 7);
    tree_add(&tree, 777, 16);
    tree_add(&tree, 1793, 16);
    tree_add(&tree, 1386, 13);

    const size_t lSize = tree_size(&tree);
    assert_int_equal( lSize, 4 );

    const size_t depth = tree_depth(&tree);
    assert_int_equal( depth, 3 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_size_NULL(void **state) {
    (void) state; /* unused */

    const size_t ret = tree_size(NULL);
    assert_int_equal( ret, 0 );
}

static void tree_depth_NULL(void **state) {
    (void) state; /* unused */

    const size_t ret = tree_depth(NULL);
    assert_int_equal( ret, 0 );
}

static void tree_depth_0(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    const size_t ret = tree_depth(&tree);
    assert_int_equal( ret, 0 );

    tree_release(&tree);
}

static void tree_startAddress_valid(void **state) {
    (void) state; /* unused */

    {
        const size_t ret = tree_startAddress(NULL);
        assert_int_equal( ret, 0 );
    }
    {
        RBTree tree;
        tree_init(&tree);

        const size_t ret = tree_startAddress(&tree);
        assert_int_equal( ret, 0 );

        tree_release(&tree);
    }
    {
        RBTree tree;
        tree_init(&tree);
        tree_add(&tree, 5, 10);
        tree_add(&tree, 35, 10);

        const size_t ret = tree_startAddress(&tree);
        assert_int_equal( ret, 5 );

        tree_release(&tree);
    }
}

static void tree_endAddress_valid(void **state) {
    (void) state; /* unused */

    {
        const size_t ret = tree_endAddress(NULL);
        assert_int_equal( ret, 0 );
    }
    {
        RBTree tree;
        tree_init(&tree);

        const size_t ret = tree_endAddress(&tree);
        assert_int_equal( ret, 0 );

        tree_release(&tree);
    }
    {
        RBTree tree;
        tree_init(&tree);
        tree_add(&tree, 5, 10);
        tree_add(&tree, 35, 10);

        const size_t ret = tree_endAddress(&tree);
        assert_int_equal( ret, 45 );

        tree_release(&tree);
    }
}

static void tree_release_NULL(void **state) {
    (void) state; /* unused */

    const int ret = tree_release(NULL);
    assert_int_equal( ret, -1 );
}

static void tree_release_empty(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);
    const int ret = tree_release(&tree);
    assert_int_equal( ret, 0 );
}

static void tree_release_2(void **state) {
    (void) state; /* unused */

    RBTree tree;
    const int init = tree_init(&tree);
    assert_int_equal( init, 0 );

    tree_add(&tree, 1, 1);
    tree_add(&tree, 2, 1);

    const int ret = tree_release(&tree);
    assert_int_equal( ret, 2 );
}

static void tree_randomTest1(void **state) {
    (void) state; /* unused */

    const unsigned int seed = time(NULL);
    /// const unsigned int seed = 0;
    srand( seed );

    RBTree tree;
    tree_init(&tree);

    for(size_t i = 0; i< 100; ++i) {
        const size_t addr = rand() % 2000;
        const size_t msize = rand() % 20 +1;

        /// printf("Iteration %lu: adding (%lu, %lu)\n", i, addr, msize);

        tree_add(&tree, addr, msize);

        const int valid = tree_isValid(&tree);
        assert_int_equal( valid, 0 );
    }

    const size_t startAddress = tree_startAddress(&tree);
    const size_t endAddress = tree_endAddress(&tree);
    const size_t addressSpace = endAddress - startAddress;

//    for(size_t i = 0; i< 100; ++i) {
//        const size_t addr = rand() % addressSpace + startAddress;
//
//        tree_delete(&tree, addr);
//
//        printf("Iteration %lu\n", i);
//        const int valid = tree_isValid(&tree);
//        assert_int_equal( valid, 0 );
//    }

    ///printf("Releasing\n");
    tree_release(&tree);
}


/// ==================================================


int main(void) {
    const struct UnitTest tests[] = {
        unit_test(tree_add_NULL),
        unit_test(tree_add_left),
        unit_test(tree_add_right),
        unit_test(tree_add_subtree_left),
        unit_test(tree_add_subtree_right),
        unit_test(tree_add_subtree),
        unit_test(tree_add_subtree_space),
        unit_test(tree_add_subtree_startAddr),

        unit_test(tree_size_NULL),
        unit_test(tree_depth_NULL),
        unit_test(tree_depth_0),
        unit_test(tree_startAddress_valid),
        unit_test(tree_endAddress_valid),

        unit_test(tree_release_NULL),
        unit_test(tree_release_empty),
        unit_test(tree_release_2),

        unit_test(tree_mmap_NULL),
        unit_test(tree_mmap_first),
        unit_test(tree_mmap_second),
        unit_test(tree_mmap_segmented_toLeft),
        unit_test(tree_mmap_segmented_toRight),

        unit_test(tree_munmap_NULL),
        unit_test(tree_munmap_empty),
        unit_test(tree_munmap_badaddr),
        unit_test(tree_munmap_root),
        unit_test(tree_munmap_root2),
        unit_test(tree_munmap_right),
        unit_test(tree_munmap_right2),
        unit_test(tree_munmap_left),
        unit_test(tree_munmap_left2),

        unit_test(tree_init_NULL),
        unit_test(tree_init_valid),

        unit_test(tree_randomTest1),
    };

    return run_group_tests(tests);
}
