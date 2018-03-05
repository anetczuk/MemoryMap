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

    tree_release(&tree);
}

static void tree_mmap_second(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)160, 64);

    const void* ret = tree_mmap(&tree, (void*)128, 64);
    assert_int_equal( ret, 224 );

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

    tree_release(&tree);
}

static void tree_munmap_freed(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_mmap(&tree, (void*)100, 64);
    tree_mmap(&tree, (void*)200, 64);

    tree_munmap(&tree, (void*)120);

    const size_t ret = tree_size(&tree);
    assert_int_equal( ret, 1 );

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
}


/// ==================================================


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

static void tree_add_left(void **state) {
    (void) state; /* unused */

    RBTree tree;
    tree_init(&tree);

    tree_add(&tree, 3, 1);
    tree_add(&tree, 1, 1);

    const size_t lSize = tree_size(&tree);
    assert_int_equal( lSize, 2 );

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

    tree_release(&tree);
}

//static void tree_add_subtree2(void **state) {
//    (void) state; /* unused */
//
//    RBTree tree;
//    tree_init(&tree);
//
//    const size_t a1 = tree_add(&tree, 3, 1);
//    assert_int_equal( a1, 3 );
//
//    const size_t lSize = tree_size(&tree);
//    assert_int_equal( lSize, 10 );
//
//    tree_release(&tree);
//}


/// ==================================================


int main(void) {
    const struct UnitTest tests[] = {
        unit_test(tree_release_NULL),
        unit_test(tree_release_empty),
        unit_test(tree_release_2),
        unit_test(tree_add_left),
        unit_test(tree_add_right),
        unit_test(tree_add_subtree),

        unit_test(tree_mmap_NULL),
        unit_test(tree_mmap_first),
        unit_test(tree_mmap_second),
        unit_test(tree_mmap_segmented_toLeft),
        unit_test(tree_mmap_segmented_toRight),

        unit_test(tree_munmap_NULL),
        unit_test(tree_munmap_empty),
        unit_test(tree_munmap_badaddr),
        unit_test(tree_munmap_freed),

        unit_test(tree_init_NULL),
        unit_test(tree_init_valid)
    };

    return run_group_tests(tests);
}
