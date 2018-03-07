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


static RBTree create_default_tree(const size_t nodes) {
    RBTree tree;
    tree_init(&tree);

    for(size_t i = 0; i < 16; ++i) {
        tree_add(&tree, 1, 1);

        const size_t tSize = tree_size(&tree);
        assert_int_equal( tSize, i+1 );

        const int valid = tree_isValid(&tree);
        assert_int_equal( valid, 0 );
    }

    return tree;
}

static RBTree create_random_tree(const unsigned int seed, const size_t nodes) {
    srand( seed );

    RBTree tree;
    tree_init(&tree);

    for(size_t i = 0; i < nodes; ++i) {
        const size_t addr = rand() % nodes;

        tree_add(&tree, addr, 1);

        const size_t tSize = tree_size(&tree);
        assert_int_equal( tSize, i+1 );

        const int valid = tree_isValid(&tree);
        assert_int_equal( valid, 0 );
    }

    return tree;
}


/// ======================================================


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
    assert_int_equal( ret, 3 );

    const size_t depth = tree_depth(&tree);
    assert_int_equal( depth, 2 );

    assert_int_equal( tree_isValid(&tree), 0 );

    tree_release(&tree);
}

static void tree_munmap_subtree(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)20, 18);
    tree_mmap(&tree, (void*)92, 17);
    tree_mmap(&tree, (void*)72, 19);
    tree_mmap(&tree, (void*)106, 16);
    tree_mmap(&tree, (void*)90, 12);
    tree_mmap(&tree, (void*)46, 16);
    tree_mmap(&tree, (void*)92, 17);        /// the same once again
    tree_mmap(&tree, (void*)113, 13);
    tree_mmap(&tree, (void*)155, 13);
    tree_mmap(&tree, (void*)110, 18);

    assert_int_equal( tree_size(&tree), 10 );

    tree_munmap(&tree, (void*)96);

    const size_t ret = tree_size(&tree);
    assert_int_equal( ret, 9 );

    const size_t depth = tree_depth(&tree);
    assert_int_equal( depth, 4 );

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

static void test_node_index(void **state) {
    (void) state; /* unused */

    const size_t treeSize = 16;
    RBTree tree = create_default_tree(treeSize);

    for(size_t i = 0; i < treeSize; ++i) {
        RBTreeNode* node = tree_findNode(&tree, i+1);
        assert_non_null( node );

        const size_t ind = node_index(node);
        assert_int_equal( ind+1, node->area.start );
    }

    tree_release(&tree);
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

static void tree_delete_1(void **state) {
    (void) state; /* unused */

    RBTree tree = create_random_tree(0, 16);

    /// tree_print(&tree);

    tree_delete(&tree, 1);

    const int valid = tree_isValid(&tree);
    assert_int_equal( valid, 0 );

    ///printf("Releasing\n");
    tree_release(&tree);
}

static void tree_randomTest1(void **state) {
    (void) state; /* unused */

    const unsigned int seed = time(NULL);
    /// const unsigned int seed = 0;
    srand( seed );

    RBTree tree;
    tree_init(&tree);

    static const size_t nodes_num = 10;

    for(size_t i = 0; i < nodes_num; ++i) {
        const size_t addr = rand() % 200;
        const size_t msize = rand() % 20 +1;

        /// printf("Iteration %lu: adding (%lu, %lu)\n", i, addr, msize);

        tree_add(&tree, addr, msize);

        const size_t tSize = tree_size(&tree);
        assert_int_equal( tSize, i+1 );

        const int valid = tree_isValid(&tree);
        assert_int_equal( valid, 0 );
    }

    const size_t startAddress = tree_startAddress(&tree);
    const size_t endAddress = tree_endAddress(&tree);
    const size_t addressSpace = endAddress - startAddress;

    for(size_t i = 0; i < nodes_num; ++i) {
        const size_t addr = rand() % addressSpace + startAddress;

        /// printf("Iteration %lu: removing %lu\n", i, addr);

        tree_delete(&tree, addr);

        const int valid = tree_isValid(&tree);
        assert_int_equal( valid, 0 );
    }

    ///printf("Releasing\n");
    tree_release(&tree);
}

static void tree_randomTest2(void **state) {
    (void) state; /* unused */

    const unsigned int seed = time(NULL);
    /// const unsigned int seed = 0;
    srand( seed );

    RBTree tree;
    tree_init(&tree);

    static const size_t nodes_num = 10;

    for(size_t i = 0; i < nodes_num; ++i) {
        const size_t addr = rand() % 2000;
        const size_t msize = rand() % 20 +1;

        /// printf("Iteration %lu: adding (%lu, %lu)\n", i, addr, msize);

        tree_add(&tree, addr, msize);

        const size_t tSize = tree_size(&tree);
        assert_int_equal( tSize, i+1 );

        const int valid = tree_isValid(&tree);
        assert_int_equal( valid, 0 );
    }

    const size_t startAddress = tree_startAddress(&tree);
    const size_t endAddress = tree_endAddress(&tree);
    const size_t addressSpace = endAddress - startAddress;

    for(size_t i = 0; i < nodes_num; ++i) {
        const size_t addr = rand() % addressSpace + startAddress;

        /// printf("Iteration %lu: removing %lu\n", i, addr);

        tree_delete(&tree, addr);

        const int valid = tree_isValid(&tree);
        assert_int_equal( valid, 0 );
    }

    ///printf("Releasing\n");
    tree_release(&tree);
}


/// ==================================================


int main(void) {

    //TODO: add selective run

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

        unit_test(test_node_index),

        unit_test(tree_release_NULL),
        unit_test(tree_release_empty),
        unit_test(tree_release_2),

        unit_test(tree_delete_1),

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
        unit_test(tree_munmap_subtree),

        unit_test(tree_init_NULL),
        unit_test(tree_init_valid),

        unit_test(tree_randomTest1),
        unit_test(tree_randomTest2),
    };

///    return run_test( test_delete_1 );

    return run_group_tests(tests);
}
