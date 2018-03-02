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

#include "mymap/LinkedList.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>


static void list_mmap_param_NULL(void **state) {
    (void) state; /* unused */

    const void* ret = list_mmap(NULL, NULL, 0, 0, NULL);

    assert_null( ret );
}

static void list_mmap_first(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);

    const void* ret = list_mmap(&list, (void*)128, 64, 0, NULL);
    assert_int_equal( ret, 128 );

    list_release(&list);
}

static void list_mmap_second(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);

    list_mmap(&list, (void*)160, 64, 0, NULL);

    const void* ret = list_mmap(&list, (void*)128, 64, 0, NULL);
    assert_int_equal( ret, 224 );

    list_release(&list);
}

static void list_munmap_param_NULL(void **state) {
    (void) state; /* unused */

    list_munmap(NULL, NULL);

    assert_true( 1 );
}

static void list_init_param_NULL(void **state) {
    (void) state; /* unused */

    const int ret = list_init(NULL);
    assert_int_equal( ret, -1 );
}

static void list_init_valid(void **state) {
    (void) state; /* unused */

    LinkedList list;
    const int ret = list_init(&list);
    assert_int_equal( ret, 0 );
}


/// ==================================================


static void list_release_param_NULL(void **state) {
    (void) state; /* unused */

    const int ret = list_release(NULL);
    assert_int_equal( ret, -1 );
}

static void list_release_list(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);
    const int ret = list_release(&list);
    assert_int_equal( ret, 0 );
}

static void list_release_2(void **state) {
    (void) state; /* unused */

    LinkedList list;
    const int init = list_init(&list);
    assert_int_equal( init, 0 );

    list_addValue(&list, 1);
    list_addValue(&list, 2);

    const int ret = list_release(&list);
    assert_int_equal( ret, 2 );
}

static void list_size_NULL(void **state) {
    (void) state; /* unused */

    const size_t ret = list_size(NULL);
    assert_int_equal( ret, 0 );
}

static void list_size_0(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);

    const size_t ret = list_size(&list);
    assert_int_equal( ret, 0 );

    list_release(&list);
}

static void list_addValue_first(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);
    list_addValue(&list, 3);
    list_addValue(&list, 1);

    const size_t lSize = list_size(&list);
    assert_int_equal( lSize, 2 );

    const int val = list_getValue(&list, 0);
    assert_int_equal( val, 1 );

    list_release(&list);
}

static void list_addValue_last(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);
    list_addValue(&list, 1);
    list_addValue(&list, 5);

    const size_t lSize = list_size(&list);
    assert_int_equal( lSize, 2 );

    const int val = list_getValue(&list, 1);
    assert_int_equal( val, 5 );

    list_release(&list);
}

static void list_addValue_middle(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);
    list_addValue(&list, 1);
    list_addValue(&list, 5);
    list_addValue(&list, 3);

    const size_t lSize = list_size(&list);
    assert_int_equal( lSize, 3 );

    const int val = list_getValue(&list, 1);
    assert_int_equal( val, 3 );

    list_release(&list);
}


/// ==================================================


int main(void) {
    const struct UnitTest tests[] = {
        unit_test(list_release_param_NULL),
        unit_test(list_release_list),
        unit_test(list_release_2),
        unit_test(list_addValue_first),
        unit_test(list_addValue_middle),
        unit_test(list_addValue_last),
        unit_test(list_size_NULL),
        unit_test(list_size_0),

        unit_test(list_mmap_param_NULL),
        unit_test(list_mmap_first),
        unit_test(list_mmap_second),
        unit_test(list_munmap_param_NULL),
        unit_test(list_init_param_NULL),
        unit_test(list_init_valid)
    };

    return run_group_tests(tests);
}
