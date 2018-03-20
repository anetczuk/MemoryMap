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


static unsigned int current_seed = 0;

static unsigned int get_next_seed() {
	if (current_seed == 0) {
		current_seed = time(NULL);
	}
	return (--current_seed);
}

static RBTree create_default_tree(const size_t nodes) {
    RBTree tree;
    tree_init(&tree);

    for(size_t i = 0; i < nodes; ++i) {
        tree_add(&tree, i+1, 1);

//        assert_int_equal( tree_size(&tree), i+1 );
//        assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );
    }

    return tree;
}

static RBTree create_random_tree(const unsigned int seed, const size_t nodes) {
    srand( seed );

    RBTree tree;
    tree_init(&tree);

    for(size_t i = 0; i < nodes; ++i) {
        const size_t addr = rand() % nodes +1;
        tree_add(&tree, addr, 1);

        assert_int_equal( tree_size(&tree), i+1 );
        assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );
    }

    return tree;
}

static RBTree create_random_tree_map(const unsigned int seed, const size_t nodes, const size_t addressRange, const size_t sizeRange) {
    srand( seed );

    RBTree tree;
    tree_init(&tree);

    for(size_t i = 0; i < nodes; ++i) {
        const size_t addr = rand() % addressRange +1;
        const size_t msize = rand() % sizeRange +1;

        tree_add(&tree, addr, msize);

//        printf("Iteration %lu: adding (%lu, %lu), size: %lu\n", i, addr, msize, tree_size(&tree) );

//        assert_int_equal( tree_size(&tree), i+1 );
//        assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );
    }

    return tree;
}


/// ======================================================


static void test_tree_mmap_NULL(void **state) {
    (void) state; /* unused */

    const void* ret = tree_mmap(NULL, NULL, 0);

    assert_null( ret );
}

static void test_tree_mmap_first(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    const void* ret = tree_mmap(&tree, (void*)128, 64);
    assert_int_equal( ret, 128 );

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_mmap_second(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)160, 64);

    const void* ret = tree_mmap(&tree, (void*)128, 64);
    assert_int_equal( ret, 224 );

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_mmap_segmented_toLeft(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)200, 64);
    /// small space between segments
    tree_mmap(&tree, (void*)100, 64);

    const void* ret = tree_mmap(&tree, (void*)128, 64);
    assert_int_equal( ret, 264 );

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_mmap_segmented_toRight(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)100, 64);
    /// small space between segments
    tree_mmap(&tree, (void*)200, 64);

    const void* ret = tree_mmap(&tree, (void*)128, 64);
    assert_int_equal( ret, 264 );

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_munmap_NULL(void **state) {
    (void) state; /* unused */

    tree_munmap(NULL, NULL);

    assert_true( 1 );
}

static void test_tree_munmap_empty(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_munmap(&tree, NULL);

    tree_release(&tree);
}

static void test_tree_munmap_badaddr(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);
    tree_mmap(&tree, (void*)100, 64);

    tree_munmap(&tree, (void*)50);

    const size_t ret = tree_size(&tree);
    assert_int_equal( ret, 1 );

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_munmap_root(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)100, 64);
    tree_mmap(&tree, (void*)200, 64);

    tree_munmap(&tree, (void*)120);

    const size_t ret = tree_size(&tree);
    assert_int_equal( ret, 1 );

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_munmap_root2(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)100, 64);
    tree_mmap(&tree, (void*)20, 64);
    tree_mmap(&tree, (void*)200, 64);

    tree_munmap(&tree, (void*)120);

    const size_t ret = tree_size(&tree);
    assert_int_equal( ret, 2 );

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_munmap_right(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)100, 64);
    tree_mmap(&tree, (void*)200, 64);

    tree_munmap(&tree, (void*)220);

    const size_t ret = tree_size(&tree);
    assert_int_equal( ret, 1 );

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_munmap_right2(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)100, 64);
    tree_mmap(&tree, (void*)20, 64);
    tree_mmap(&tree, (void*)200, 64);

    tree_munmap(&tree, (void*)220);

    const size_t ret = tree_size(&tree);
    assert_int_equal( ret, 2 );

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_munmap_left(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)200, 64);
    tree_mmap(&tree, (void*)100, 64);
    tree_mmap(&tree, (void*)300, 64);

    tree_munmap(&tree, (void*)120);

    const size_t ret = tree_size(&tree);
    assert_int_equal( ret, 2 );

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_munmap_left2(void **state) {
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

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_munmap_subtree(void **state) {
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

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_init_NULL(void **state) {
    (void) state; /* unused */

    const int ret = tree_init(NULL);
    assert_int_equal( ret, -1 );
}

static void test_tree_init_valid(void **state) {
    (void) state; /* unused */

    RBTree tree;
    const int ret = tree_init(&tree);
    assert_int_equal( ret, 0 );

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}


/// ==================================================


static void test_tree_add_NULL(void **state) {
    (void) state; /* unused */

    const size_t ret = tree_add(NULL, 3, 1);
    assert_int_equal( ret, 0 );
}

static void test_tree_add_0(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    const size_t retAddr = tree_add(&tree, 0, 1);
    assert_int_equal( retAddr, 0 );

    const size_t lSize = tree_size(&tree);
    assert_int_equal( lSize, 0 );

    const size_t depth = tree_depth(&tree);
    assert_int_equal( depth, 0 );

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_add_left(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_add(&tree, 3, 1);
    tree_add(&tree, 1, 1);

    const size_t lSize = tree_size(&tree);
    assert_int_equal( lSize, 2 );

    const size_t depth = tree_depth(&tree);
    assert_int_equal( depth, 2 );

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_add_right(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_add(&tree, 3, 1);
    tree_add(&tree, 6, 1);

    const size_t lSize = tree_size(&tree);
    assert_int_equal( lSize, 2 );

    const size_t depth = tree_depth(&tree);
    assert_int_equal( depth, 2 );

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_add_subtree_left(void **state) {
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

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_add_subtree_right(void **state) {
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

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_add_subtree(void **state) {
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

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_add_subtree_space(void **state) {
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

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_add_subtree_startAddr(void **state) {
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

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}
static void test_tree_add_subtree1(void **state) {
    (void) state; /* unused */

    const unsigned int seed = 1520466046;
    const size_t nodes_num = 16;

    RBTree tree = create_random_tree_map(seed, nodes_num, 200, 20);

    assert_int_equal( tree_size(&tree), nodes_num );
    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_size_NULL(void **state) {
    (void) state; /* unused */

    const size_t ret = tree_size(NULL);
    assert_int_equal( ret, 0 );
}

static void test_tree_depth_NULL(void **state) {
    (void) state; /* unused */

    const size_t ret = tree_depth(NULL);
    assert_int_equal( ret, 0 );
}

static void test_tree_depth_0(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    const size_t ret = tree_depth(&tree);
    assert_int_equal( ret, 0 );

    tree_release(&tree);
}

static void test_tree_startAddress_valid(void **state) {
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

static void test_tree_endAddress_valid(void **state) {
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

static void test_tree_area_NULL(void **state) {
    (void) state; /* unused */

	const MemoryArea area = tree_area(NULL);
	assert_int_equal( area.start, 0 );
	assert_int_equal( area.end, 0 );
}

static void test_tree_area_empty(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

	const MemoryArea area = tree_area(&tree);

	assert_int_equal( area.start, 0 );
	assert_int_equal( area.end, 0 );

    tree_release(&tree);
}

static void test_tree_isValid_NULL(void **state) {
    (void) state; /* unused */

    assert_int_equal( tree_isValid(NULL), RBTREE_INVALID_OK );
}

static void test_tree_isValid_valid(void **state) {
    (void) state; /* unused */

    const size_t treeSize = 22;
    RBTree tree = create_default_tree(treeSize);

    assert_int_equal( tree_size(&tree), treeSize );

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_nodeIndex_NULL(void **state) {
	(void) state; /* unused */

	const size_t ind = tree_nodeIndex(NULL);
	assert_int_equal( ind, -1 );
}

static void test_tree_nodeIndex(void **state) {
    (void) state; /* unused */

    const size_t treeSize = 16;
    RBTree tree = create_default_tree(treeSize);

    /// tree_print(&tree);

    assert_int_equal( tree_size(&tree), treeSize );
    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    for(size_t i = 0; i < treeSize; ++i) {
        RBTreeNode* node = tree_findNode(&tree, i+1);
        assert_non_null( node );

        const size_t ind = tree_nodeIndex(node);
        assert_int_equal( ind+1, node->area.start );
    }

    tree_release(&tree);
}

static void test_tree_release_NULL(void **state) {
    (void) state; /* unused */

    const int ret = tree_release(NULL);
    assert_int_equal( ret, -1 );
}

static void test_tree_release_empty(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);
    const int ret = tree_release(&tree);
    assert_int_equal( ret, 0 );
}

static void test_tree_release_double(void **state) {
    (void) state; /* unused */

    RBTree memMap;
    tree_init(&memMap);

    tree_add(&memMap, 10, 10);

    assert_int_equal( tree_release(&memMap), 1 );
    assert_int_equal( tree_release(&memMap), 0 );
}

static void test_tree_release_2(void **state) {
    (void) state; /* unused */

    RBTree tree;
    const int init = tree_init(&tree);
    assert_int_equal( init, 0 );

    tree_add(&tree, 1, 1);
    tree_add(&tree, 2, 1);

    const int ret = tree_release(&tree);
    assert_int_equal( ret, 2 );
}

static void test_tree_delete_root(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_add(&tree, 10, 1);
    tree_add(&tree,  5, 1);

    tree_delete(&tree, 10);			/// deleting root

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_delete_R_both(void **state) {
    (void) state; /* unused */

    const size_t treeSize = 16;
    RBTree tree = create_random_tree(9, treeSize);

    assert_int_equal( tree_size(&tree), treeSize );
    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    /// tree_print(&tree);

    tree_delete(&tree, 10);

    /// tree_print(&tree);

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_delete_R2(void **state) {
    (void) state; /* unused */

    const size_t treeSize = 16;
    RBTree tree = create_random_tree(1520453854, treeSize);

    assert_int_equal( tree_size(&tree), treeSize );
    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

//    tree_print(&tree);

    const MemoryArea area = tree_area(&tree);
    const size_t addr = rand() % memory_size(&area) + area.start;

//    printf("seed: %u deleting: %lu\n", seed, addr);

    tree_delete(&tree, addr);

//    tree_print(&tree);

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_delete_R2_L(void **state) {
    (void) state; /* unused */

    const size_t treeSize = 16;
    RBTree tree = create_random_tree(1420453862, treeSize);

    assert_int_equal( tree_size(&tree), treeSize );
    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

//    tree_print(&tree);

    const MemoryArea area = tree_area(&tree);
    const size_t addr = rand() % memory_size(&area) + area.start;

//    printf("seed: %u deleting: %lu\n", seed, addr);

    tree_delete(&tree, addr);

//    tree_print(&tree);

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_delete_R3(void **state) {
    (void) state; /* unused */

    const size_t treeSize = 16;
    RBTree tree = create_random_tree(1520461234, treeSize);

    assert_int_equal( tree_size(&tree), treeSize );
    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

//    tree_print(&tree);

    const MemoryArea area = tree_area(&tree);
    const size_t addr = rand() % memory_size(&area) + area.start;

//    printf("seed: %u deleting: %lu\n", seed, addr);

    tree_delete(&tree, addr);

//    tree_print(&tree);

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_delete_R3b(void **state) {
    (void) state; /* unused */

    const size_t treeSize = 16;
    RBTree tree = create_random_tree(1320973508, treeSize);

    assert_int_equal( tree_size(&tree), treeSize );
    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

//    tree_print(&tree);

    const MemoryArea area = tree_area(&tree);
    const size_t addr = rand() % memory_size(&area) + area.start;

//    printf("seed: %u deleting: %lu\n", seed, addr);

    tree_delete(&tree, addr);

//    tree_print(&tree);

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_delete_R3c(void **state) {
    (void) state; /* unused */

    const size_t treeSize = 16;
    RBTree tree = create_random_tree(1520975311, treeSize);

    assert_int_equal( tree_size(&tree), treeSize );
    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

//    tree_print(&tree);

    const MemoryArea area = tree_area(&tree);
    const size_t addr = rand() % memory_size(&area) + area.start;

//    printf("seed: %u deleting: %lu\n", seed, addr);

    tree_delete(&tree, addr);

//    tree_print(&tree);

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_delete_R4(void **state) {
    (void) state; /* unused */

    const size_t treeSize = 16;
    RBTree tree = create_random_tree(1520461635, treeSize);

    assert_int_equal( tree_size(&tree), treeSize );
    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

//    tree_print(&tree);

    const MemoryArea area = tree_area(&tree);
    const size_t addr = rand() % memory_size(&area) + area.start;

//    printf("seed: %u deleting: %lu\n", seed, addr);

    tree_delete(&tree, addr);

//    tree_print(&tree);

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_delete_R5(void **state) {
    (void) state; /* unused */

    const size_t treeSize = 16;
    RBTree tree = create_random_tree(1520975904, treeSize);

    assert_int_equal( tree_size(&tree), treeSize );
    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

//    tree_print(&tree);

    const MemoryArea area = tree_area(&tree);
    const size_t addr = rand() % memory_size(&area) + area.start;

//    printf("seed: %u deleting: %lu\n", seed, addr);

    tree_delete(&tree, addr);

//    tree_print(&tree);

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_delete_random(void **state) {
    (void) state; /* unused */

    const unsigned int seed = get_next_seed();
    const size_t treeSize = 16;
    RBTree tree = create_random_tree(seed, treeSize);

    assert_int_equal( tree_size(&tree), treeSize );
    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    /// tree_print(&tree);

    const MemoryArea area = tree_area(&tree);
    const size_t addr = rand() % memory_size(&area) + area.start;

//    printf("seed: %u deleting: %lu\n", seed, addr);

    tree_delete(&tree, addr);

    /// tree_print(&tree);

    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    tree_release(&tree);
}

static void test_tree_findNode_NULL(void **state) {
    (void) state; /* unused */

	const RBTreeNode* node = tree_findNode(NULL, 0);
	assert_null( node );
}

static void test_tree_randomT1(void **state) {
    (void) state; /* unused */

    const unsigned int seed = 1520463781;
    static const size_t nodes_num = 10;

    RBTree tree = create_random_tree_map(seed, nodes_num, 200, 20);

    const MemoryArea area = tree_area(&tree);

    for(size_t i = 0; i < nodes_num; ++i) {
        const size_t addr = rand() % memory_size(&area) + area.start;

//        tree_print(&tree);

        /// printf("Iteration %lu: removing %lu\n", i, addr);

        tree_delete(&tree, addr);

        assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );
    }

    ///printf("Releasing\n");
    tree_release(&tree);
}

static void test_tree_randomT2(void **state) {
    (void) state; /* unused */

    const unsigned int seed = 1520465042;
    static const size_t nodes_num = 10;

    RBTree tree = create_random_tree_map(seed, nodes_num, 200, 20);

    const MemoryArea area = tree_area(&tree);

    for(size_t i = 0; i < nodes_num; ++i) {
        const size_t addr = rand() % memory_size(&area) + area.start;

//        tree_print(&tree);

        /// printf("Iteration %lu: removing %lu\n", i, addr);

        tree_delete(&tree, addr);

        assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );
    }

    ///printf("Releasing\n");
    tree_release(&tree);
}

static void test_tree_randomTest1(void **state) {
    (void) state; /* unused */

    const unsigned int seed = get_next_seed();
//    printf("seed: %u\n", seed);
    static const size_t nodes_num = 10;

    RBTree tree = create_random_tree_map(seed, nodes_num, 200, 20);

    const size_t treeSize = tree_size(&tree);
    if (treeSize != nodes_num) {
        printf("seed: %u\n", seed);
    }
    assert_int_equal( treeSize, nodes_num );
    assert_int_equal( tree_isValid(&tree), RBTREE_INVALID_OK );

    const MemoryArea area = tree_area(&tree);

    for(size_t i = 0; i < nodes_num; ++i) {
        const size_t addr = rand() % memory_size(&area) + area.start;

        /// printf("Iteration %lu: removing %lu\n", i, addr);

        tree_delete(&tree, addr);

        const RBTreeValidationError valid = tree_isValid(&tree);
        if (valid != RBTREE_INVALID_OK) {
            printf("seed: %u\n", seed);
            printf("Iteration %zu: removing %zu\n", i, addr);
        }
        assert_int_equal( valid, RBTREE_INVALID_OK );
    }

    ///printf("Releasing\n");
    tree_release(&tree);
}

static void test_tree_randomTest2(void **state) {
    (void) state; /* unused */

    const unsigned int seed = get_next_seed();
//    printf("seed: %u\n", seed);
    static const size_t nodes_num = 10;

    RBTree tree = create_random_tree_map(seed, nodes_num, 2000, 20);

    const size_t treeSize = tree_size(&tree);
    if (treeSize != nodes_num) {
        printf("seed: %u\n", seed);
    }
    assert_int_equal( treeSize, nodes_num );

    const MemoryArea area = tree_area(&tree);

    for(size_t i = 0; i < nodes_num; ++i) {
        const size_t addr = rand() % memory_size(&area) + area.start;

        /// printf("Iteration %lu: removing %lu\n", i, addr);

        tree_delete(&tree, addr);

        const RBTreeValidationError valid = tree_isValid(&tree);
        if (valid != RBTREE_INVALID_OK) {
            printf("seed: %u\n", seed);
            printf("Iteration %zu: removing %zu\n", i, addr);
        }
        assert_int_equal( valid, RBTREE_INVALID_OK );
    }

    ///printf("Releasing\n");
    tree_release(&tree);
}


/// ==================================================


int main(void) {

    //TODO: add selective run

    const struct UnitTest tests[] = {
        unit_test(test_tree_add_NULL),
        unit_test(test_tree_add_0),
        unit_test(test_tree_add_left),
        unit_test(test_tree_add_right),
        unit_test(test_tree_add_subtree_left),
        unit_test(test_tree_add_subtree_right),
        unit_test(test_tree_add_subtree),
        unit_test(test_tree_add_subtree_space),
        unit_test(test_tree_add_subtree_startAddr),
        unit_test(test_tree_add_subtree1),

        unit_test(test_tree_size_NULL),
        unit_test(test_tree_depth_NULL),
        unit_test(test_tree_depth_0),
        unit_test(test_tree_startAddress_valid),
        unit_test(test_tree_endAddress_valid),
        unit_test(test_tree_area_NULL),
        unit_test(test_tree_area_empty),

        unit_test(test_tree_isValid_NULL),
        unit_test(test_tree_isValid_valid),

        unit_test(test_tree_nodeIndex_NULL),
        unit_test(test_tree_nodeIndex),

        unit_test(test_tree_release_NULL),
        unit_test(test_tree_release_empty),
        unit_test(test_tree_release_double),
        unit_test(test_tree_release_2),

        unit_test(test_tree_delete_root),
        unit_test(test_tree_delete_R_both),
        unit_test(test_tree_delete_R2),
        unit_test(test_tree_delete_R2_L),
        unit_test(test_tree_delete_R3),
        unit_test(test_tree_delete_R3b),
        unit_test(test_tree_delete_R3c),
        unit_test(test_tree_delete_R4),
        unit_test(test_tree_delete_R5),
        unit_test(test_tree_delete_random),

		unit_test(test_tree_findNode_NULL),

        unit_test(test_tree_mmap_NULL),
        unit_test(test_tree_mmap_first),
        unit_test(test_tree_mmap_second),
        unit_test(test_tree_mmap_segmented_toLeft),
        unit_test(test_tree_mmap_segmented_toRight),

        unit_test(test_tree_munmap_NULL),
        unit_test(test_tree_munmap_empty),
        unit_test(test_tree_munmap_badaddr),
        unit_test(test_tree_munmap_root),
        unit_test(test_tree_munmap_root2),
        unit_test(test_tree_munmap_right),
        unit_test(test_tree_munmap_right2),
        unit_test(test_tree_munmap_left),
        unit_test(test_tree_munmap_left2),
        unit_test(test_tree_munmap_subtree),

        unit_test(test_tree_init_NULL),
        unit_test(test_tree_init_valid),

        unit_test(test_tree_randomT1),
        unit_test(test_tree_randomT2),
        unit_test(test_tree_randomTest1),
        unit_test(test_tree_randomTest2),
    };

    return run_group_tests(tests);
}
