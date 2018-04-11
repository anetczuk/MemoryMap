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
        srand( time(NULL) );
        current_seed = rand();
    }
    return (++current_seed);
}

static UIntRBTree create_default_tree(const size_t nodes) {
    UIntRBTree tree;
    uirbtree_init(&tree);

    for(size_t i = 0; i < nodes; ++i) {
    	uirbtree_add(&tree, i+1);

//        assert_int_equal( uirbtree_size(&tree), i+1 );
//        assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );
    }

    return tree;
}

static UIntRBTree create_random_uirbtree_map(const unsigned int seed, const size_t nodes, const size_t valueRange) {
    srand( seed );

    UIntRBTree tree;
    uirbtree_init(&tree);

    for(size_t i = 0; i < nodes; ++i) {
        const size_t val = rand() % valueRange +1;

        uirbtree_add(&tree, val);

//        printf("Iteration %lu: adding (%lu, %lu), size: %lu\n", i, addr, msize, uirbtree_size(&tree) );

//        assert_int_equal( uirbtree_size(&tree), i+1 );
//        assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );
    }

    return tree;
}


/// ======================================================


static void test_uirbtree_init_NULL(void **state) {
    (void) state; /* unused */

    const bool ret = uirbtree_init(NULL);
    assert_int_equal( ret, false );
}

static void test_uirbtree_init_valid(void **state) {
    (void) state; /* unused */

    UIntRBTree tree;
    const bool ret = uirbtree_init(&tree);
    assert_int_equal( ret, true );

    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    uirbtree_release(&tree);
}


/// ==================================================


static void test_uirbtree_add_NULL(void **state) {
    (void) state; /* unused */

    const bool ret = uirbtree_add(NULL, 3);
    assert_int_equal( ret, false );
}

static void test_uirbtree_add_root(void **state) {
    (void) state; /* unused */

    UIntRBTree tree;
    uirbtree_init(&tree);

    const bool retAddr = uirbtree_add(&tree, 10);
    assert_int_equal( retAddr, true );

    assert_int_equal( uirbtree_size(&tree), 1 );
    assert_int_equal( uirbtree_depth(&tree), 1 );
    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    uirbtree_release(&tree);
}

static void test_uirbtree_add_same(void **state) {
    (void) state; /* unused */

    UIntRBTree tree;
    uirbtree_init(&tree);

    const bool retAddr1 = uirbtree_add(&tree, 10);
    assert_int_equal( retAddr1, true );

    const bool retAddr2 = uirbtree_add(&tree, 10);
    assert_int_equal( retAddr2, true );

//    uirbtree_print(&tree);

    assert_int_equal( uirbtree_size(&tree), 2 );
    assert_int_equal( uirbtree_depth(&tree), 2 );
    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    uirbtree_release(&tree);
}

static void test_uirbtree_add_left(void **state) {
    (void) state; /* unused */

    UIntRBTree tree;
    uirbtree_init(&tree);

    uirbtree_add(&tree, 3);
    uirbtree_add(&tree, 1);

//    uirbtree_print(&tree);

    assert_int_equal( uirbtree_size(&tree), 2 );
    assert_int_equal( uirbtree_depth(&tree), 2 );

    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    uirbtree_release(&tree);
}

static void test_uirbtree_add_right(void **state) {
    (void) state; /* unused */

    UIntRBTree tree;
    uirbtree_init(&tree);

    uirbtree_add(&tree, 3);
    uirbtree_add(&tree, 6);

//    uirbtree_print(&tree);

    assert_int_equal( uirbtree_size(&tree), 2 );
    assert_int_equal( uirbtree_depth(&tree), 2 );

    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    uirbtree_release(&tree);
}

static void test_uirbtree_add_subrbtree_left(void **state) {
    (void) state; /* unused */

    UIntRBTree tree;
    uirbtree_init(&tree);

    uirbtree_add(&tree, 50);
    uirbtree_add(&tree, 20);
    uirbtree_add(&tree, 30);
    uirbtree_add(&tree, 40);

    assert_int_equal( uirbtree_size(&tree), 4 );
    assert_int_equal( uirbtree_depth(&tree), 3 );

    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    uirbtree_release(&tree);
}

static void test_uirbtree_add_subrbtree_right(void **state) {
    (void) state; /* unused */

    UIntRBTree tree;
    uirbtree_init(&tree);

    uirbtree_add(&tree, 50);
    uirbtree_add(&tree, 80);
    uirbtree_add(&tree, 70);
    uirbtree_add(&tree, 60);

    assert_int_equal( uirbtree_size(&tree), 4 );
    assert_int_equal( uirbtree_depth(&tree), 3 );

    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    uirbtree_release(&tree);
}

static void test_uirbtree_add_subrbtree_right2(void **state) {
    (void) state; /* unused */

    UIntRBTree tree;
    uirbtree_init(&tree);

    uirbtree_add(&tree, 50);
    uirbtree_add(&tree, 70);
    uirbtree_add(&tree, 90);

    assert_int_equal( uirbtree_size(&tree), 3 );
    assert_int_equal( uirbtree_depth(&tree), 2 );

    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    uirbtree_release(&tree);
}

static void test_uirbtree_add_subtree(void **state) {
    (void) state; /* unused */

    UIntRBTree tree;
    uirbtree_init(&tree);

    uirbtree_add(&tree, 13);
    uirbtree_add(&tree, 8);
    uirbtree_add(&tree, 1);
    uirbtree_add(&tree, 6);
    uirbtree_add(&tree, 11);

    uirbtree_add(&tree, 17);
    uirbtree_add(&tree, 15);
    uirbtree_add(&tree, 25);
    uirbtree_add(&tree, 22);
    uirbtree_add(&tree, 27);

    assert_int_equal( uirbtree_size(&tree), 10 );
    assert_int_equal( uirbtree_depth(&tree), 4 );

//    uirbtree_print(&tree);

    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    uirbtree_release(&tree);
}

static void test_uirbtree_add_subtree1(void **state) {
    (void) state; /* unused */

    const unsigned int seed = 1520466046;
    const size_t nodes_num = 16;

    UIntRBTree tree = create_random_uirbtree_map(seed, nodes_num, 200);

    assert_int_equal( uirbtree_size(&tree), nodes_num );
    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    uirbtree_release(&tree);
}

static void test_uirbtree_size_NULL(void **state) {
    (void) state; /* unused */

    const size_t ret = uirbtree_size(NULL);
    assert_int_equal( ret, 0 );
}

static void test_uirbtree_depth_NULL(void **state) {
    (void) state; /* unused */

    const size_t ret = uirbtree_depth(NULL);
    assert_int_equal( ret, 0 );
}

static void test_uirbtree_depth_0(void **state) {
    (void) state; /* unused */

    UIntRBTree tree;
    uirbtree_init(&tree);

    const size_t ret = uirbtree_depth(&tree);
    assert_int_equal( ret, 0 );

    uirbtree_release(&tree);
}

static void test_uirbtree_isValid_NULL(void **state) {
    (void) state; /* unused */

    assert_int_equal( uirbtree_isValid(NULL), ARBTREE_INVALID_OK );
}

static void test_uirbtree_isValid_valid(void **state) {
    (void) state; /* unused */

    const size_t treeSize = 22;
    UIntRBTree tree = create_default_tree(treeSize);

    assert_int_equal( uirbtree_size(&tree), treeSize );

    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    uirbtree_release(&tree);
}

static void test_uirbtree_release_NULL(void **state) {
    (void) state; /* unused */

    const bool ret = uirbtree_release(NULL);
    assert_int_equal( ret, false );
}

static void test_uirbtree_release_empty(void **state) {
    (void) state; /* unused */

    UIntRBTree tree;
    uirbtree_init(&tree);
    const bool ret = uirbtree_release(&tree);
    assert_int_equal( ret, true );
}

static void test_uirbtree_release_double(void **state) {
    (void) state; /* unused */

    UIntRBTree memMap;
    uirbtree_init(&memMap);

    uirbtree_add(&memMap, 10);

    assert_int_equal( uirbtree_release(&memMap), true );
    assert_int_equal( uirbtree_release(&memMap), true );
}

static void test_uirbtree_release_2(void **state) {
    (void) state; /* unused */

    UIntRBTree tree;
    const bool init = uirbtree_init(&tree);
    assert_int_equal( init, true );

    uirbtree_add(&tree, 1);
    uirbtree_add(&tree, 2);

    const bool ret = uirbtree_release(&tree);
    assert_int_equal( ret, true );
}

static void test_uirbtree_delete_NULL(void **state) {
    (void) state; /* unused */

    const bool deleted = uirbtree_delete(NULL, 10);
    assert_int_equal( deleted, false );
}

static void test_uirbtree_delete_root(void **state) {
    (void) state; /* unused */

    UIntRBTree tree;
    uirbtree_init(&tree);

    uirbtree_add(&tree, 10);
    uirbtree_add(&tree,  5);

    const bool deleted = uirbtree_delete(&tree, 10);			             /// deleting root

    assert_int_equal( deleted, true );

    assert_int_equal( uirbtree_size(&tree), 1);
    assert_int_equal( uirbtree_depth(&tree), 1);
    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    uirbtree_release(&tree);
}

static void test_uirbtree_delete_root2(void **state) {
    (void) state; /* unused */

    UIntRBTree tree;
    uirbtree_init(&tree);

    uirbtree_add(&tree, 10);
    uirbtree_add(&tree, 15);

    const bool deleted = uirbtree_delete(&tree, 10);			             /// deleting root

    assert_int_equal( deleted, true );

    assert_int_equal( uirbtree_size(&tree), 1);
    assert_int_equal( uirbtree_depth(&tree), 1);
    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    uirbtree_release(&tree);
}

static void test_uirbtree_delete_item(void **state) {
    (void) state; /* unused */

    const size_t treeSize = 16;
    UIntRBTree tree = create_default_tree(treeSize);

    assert_int_equal( uirbtree_size(&tree), treeSize );
    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

//    uirbtree_print(&tree);

    const bool deleted = uirbtree_delete(&tree, 10);

//    uirbtree_print(&tree);

    assert_int_equal( deleted, true );

    assert_int_equal( uirbtree_size(&tree), 15);
    assert_int_equal( uirbtree_depth(&tree), 6);
    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    uirbtree_release(&tree);
}

static void test_uirbtree_delete_none(void **state) {
    (void) state; /* unused */

    UIntRBTree tree;
    uirbtree_init(&tree);

    uirbtree_add(&tree, 10);
    uirbtree_add(&tree,  5);

    const bool deleted = uirbtree_delete(&tree, 7);

    assert_int_equal( deleted, false );

    assert_int_equal( uirbtree_size(&tree), 2);
    assert_int_equal( uirbtree_depth(&tree), 2);
    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    uirbtree_release(&tree);
}

static void test_uirbtree_randomT1(void **state) {
    (void) state; /* unused */

    const unsigned int seed = 1681008252;
//    printf("seed: %u\n", seed);
    static const size_t nodes_num = 75;
    static const size_t max_val = 100;

    UIntRBTree tree = create_random_uirbtree_map(seed, nodes_num, max_val);

    const size_t treeSize = uirbtree_size(&tree);
    assert_int_equal( treeSize, nodes_num );
    assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

    for(size_t i = 0; i < nodes_num; ++i) {
        const size_t addr = rand() % max_val + 1;

        /// printf("Iteration %lu: removing %lu\n", i, addr);

        uirbtree_delete(&tree, addr);

        const ARBTreeValidationError valid = uirbtree_isValid(&tree);
        assert_int_equal( valid, ARBTREE_INVALID_OK );
    }

    uirbtree_release(&tree);
}

static void test_uirbtree_randomTest1(void **state) {
    (void) state; /* unused */

//    for(size_t i=0; i<1000; ++i)
    {
        const unsigned int seed = get_next_seed();
    //    printf("seed: %u\n", seed);
        static const size_t nodes_num = 75;
        static const size_t max_val = 100;

        UIntRBTree tree = create_random_uirbtree_map(seed, nodes_num, max_val);

        const size_t treeSize = uirbtree_size(&tree);
        if (treeSize != nodes_num) {
            printf("seed: %u\n", seed);
        }
        assert_int_equal( treeSize, nodes_num );
        assert_int_equal( uirbtree_isValid(&tree), ARBTREE_INVALID_OK );

        for(size_t i = 0; i < nodes_num; ++i) {
            const size_t addr = rand() % max_val + 1;

            /// printf("Iteration %lu: removing %lu\n", i, addr);

            uirbtree_delete(&tree, addr);

            const ARBTreeValidationError valid = uirbtree_isValid(&tree);
            if (valid != ARBTREE_INVALID_OK) {
                printf("seed: %u\n", seed);
                printf("Iteration %zu: removing %zu\n", i, addr);
            }
            assert_int_equal( valid, ARBTREE_INVALID_OK );
        }

        uirbtree_release(&tree);
    }
}


/// ==================================================


int main(void) {

    //TODO: add selective run

    const struct UnitTest tests[] = {
        unit_test(test_uirbtree_init_NULL),
        unit_test(test_uirbtree_init_valid),

        unit_test(test_uirbtree_size_NULL),
        unit_test(test_uirbtree_depth_NULL),
        unit_test(test_uirbtree_depth_0),

        unit_test(test_uirbtree_add_NULL),
        unit_test(test_uirbtree_add_root),
        unit_test(test_uirbtree_add_same),
        unit_test(test_uirbtree_add_left),
        unit_test(test_uirbtree_add_right),
        unit_test(test_uirbtree_add_subrbtree_left),
        unit_test(test_uirbtree_add_subrbtree_right),
        unit_test(test_uirbtree_add_subrbtree_right2),
        unit_test(test_uirbtree_add_subtree),
        unit_test(test_uirbtree_add_subtree1),

        unit_test(test_uirbtree_delete_NULL),
        unit_test(test_uirbtree_delete_root),
        unit_test(test_uirbtree_delete_root2),
        unit_test(test_uirbtree_delete_item),
        unit_test(test_uirbtree_delete_none),

        unit_test(test_uirbtree_isValid_NULL),
        unit_test(test_uirbtree_isValid_valid),

        unit_test(test_uirbtree_release_NULL),
        unit_test(test_uirbtree_release_empty),
        unit_test(test_uirbtree_release_double),
        unit_test(test_uirbtree_release_2),

        unit_test(test_uirbtree_randomT1),
        unit_test(test_uirbtree_randomTest1)
    };

    return run_group_tests(tests);
}
