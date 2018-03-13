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

#include "mymap/MyMap.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>


typedef map_t ContainerType;


static void test_mymap_mmap_NULL(void **state) {
    (void) state; /* unused */

    const void* ret = mymap_mmap(NULL, NULL, 0, 0, NULL);

    assert_null( ret );
}

static void test_mymap_mmap_empty(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    memMap.root = NULL;

    const void* ret = mymap_mmap(&memMap, (void*)128, 64, 0, NULL);
    assert_null( ret );

    mymap_release(&memMap);
}

static void test_mymap_mmap_first(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);

    const void* ret = mymap_mmap(&memMap, (void*)128, 64, 0, NULL);
    assert_int_equal( ret, 128 );

    assert_int_equal( mymap_isValid(&memMap), 0 );

    mymap_release(&memMap);
}

static void test_mymap_mmap_second(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);

    mymap_mmap(&memMap, (void*)160, 64, 0, NULL);

    const void* ret = mymap_mmap(&memMap, (void*)128, 64, 0, NULL);
    assert_int_equal( ret, 224 );

    assert_int_equal( mymap_isValid(&memMap), 0 );

    mymap_release(&memMap);
}

static void test_mymap_mmap_segmented_toLeft(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);

    mymap_mmap(&memMap, (void*)200, 64, 0, NULL);
    /// small space between segments
    mymap_mmap(&memMap, (void*)100, 64, 0, NULL);

    const void* ret = mymap_mmap(&memMap, (void*)128, 64, 0, NULL);
    assert_int_equal( ret, 264 );

    assert_int_equal( mymap_isValid(&memMap), 0 );

    mymap_release(&memMap);
}

static void test_mymap_mmap_segmented_toRight(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);

    mymap_mmap(&memMap, (void*)100, 64, 0, NULL);
    /// small space between segments
    mymap_mmap(&memMap, (void*)200, 64, 0, NULL);

    const void* ret = mymap_mmap(&memMap, (void*)128, 64, 0, NULL);
    assert_int_equal( ret, 264 );

    assert_int_equal( mymap_isValid(&memMap), 0 );

    mymap_release(&memMap);
}

static void test_mymap_munmap_NULL(void **state) {
    (void) state; /* unused */

    mymap_munmap(NULL, NULL);

    assert_true( 1 );
}

static void test_mymap_munmap_empty(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    memMap.root = NULL;

    mymap_munmap(&memMap, NULL);
}

static void test_mymap_munmap_empty2(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);

    mymap_munmap(&memMap, NULL);

    mymap_release(&memMap);
}

static void test_mymap_munmap_badaddr(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);
    mymap_mmap(&memMap, (void*)100, 64, 0, NULL);

    mymap_munmap(&memMap, (void*)50);

    const size_t ret = mymap_size(&memMap);
    assert_int_equal( ret, 1 );

    assert_int_equal( mymap_isValid(&memMap), 0 );

    mymap_release(&memMap);
}

static void test_mymap_munmap_root(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);

    mymap_mmap(&memMap, (void*)100, 64, 0, NULL);
    mymap_mmap(&memMap, (void*)200, 64, 0, NULL);

    mymap_munmap(&memMap, (void*)120);

    const size_t ret = mymap_size(&memMap);
    assert_int_equal( ret, 1 );

    assert_int_equal( mymap_isValid(&memMap), 0 );

    mymap_release(&memMap);
}

static void test_mymap_munmap_root2(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);

    mymap_mmap(&memMap, (void*)100, 64, 0, NULL);
    mymap_mmap(&memMap, (void*)20, 64, 0, NULL);
    mymap_mmap(&memMap, (void*)200, 64, 0, NULL);

    mymap_munmap(&memMap, (void*)120);

    const size_t ret = mymap_size(&memMap);
    assert_int_equal( ret, 2 );

    assert_int_equal( mymap_isValid(&memMap), 0 );

    mymap_release(&memMap);
}

static void test_mymap_munmap_right(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);

    mymap_mmap(&memMap, (void*)100, 64, 0, NULL);
    mymap_mmap(&memMap, (void*)200, 64, 0, NULL);

    mymap_munmap(&memMap, (void*)220);

    const size_t ret = mymap_size(&memMap);
    assert_int_equal( ret, 1 );

    assert_int_equal( mymap_isValid(&memMap), 0 );

    mymap_release(&memMap);
}

static void test_mymap_munmap_right2(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);

    mymap_mmap(&memMap, (void*)100, 64, 0, NULL);
    mymap_mmap(&memMap, (void*)20, 64, 0, NULL);
    mymap_mmap(&memMap, (void*)200, 64, 0, NULL);

    mymap_munmap(&memMap, (void*)220);

    const size_t ret = mymap_size(&memMap);
    assert_int_equal( ret, 2 );

    assert_int_equal( mymap_isValid(&memMap), 0 );

    mymap_release(&memMap);
}

static void test_mymap_munmap_left(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);

    mymap_mmap(&memMap, (void*)200, 64, 0, NULL);
    mymap_mmap(&memMap, (void*)100, 64, 0, NULL);
    mymap_mmap(&memMap, (void*)300, 64, 0, NULL);

    mymap_munmap(&memMap, (void*)120);

    const size_t ret = mymap_size(&memMap);
    assert_int_equal( ret, 2 );

    assert_int_equal( mymap_isValid(&memMap), 0 );

    mymap_release(&memMap);
}

static void test_mymap_munmap_left2(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);

    mymap_mmap(&memMap, (void*)200, 64, 0, NULL);
    mymap_mmap(&memMap, (void*)100, 64, 0, NULL);
    mymap_mmap(&memMap, (void*)300, 64, 0, NULL);
    mymap_mmap(&memMap, (void*)400, 64, 0, NULL);

    mymap_munmap(&memMap, (void*)320);

    const size_t ret = mymap_size(&memMap);
    assert_int_equal( ret, 3 );

    assert_int_equal( mymap_isValid(&memMap), 0 );

    mymap_release(&memMap);
}

static void test_mymap_munmap_subtree(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);

    mymap_mmap(&memMap, (void*)20, 18, 0, NULL);
    mymap_mmap(&memMap, (void*)92, 17, 0, NULL);
    mymap_mmap(&memMap, (void*)72, 19, 0, NULL);
    mymap_mmap(&memMap, (void*)106, 16, 0, NULL);
    mymap_mmap(&memMap, (void*)90, 12, 0, NULL);
    mymap_mmap(&memMap, (void*)46, 16, 0, NULL);
    mymap_mmap(&memMap, (void*)92, 17, 0, NULL);        /// the same once again
    mymap_mmap(&memMap, (void*)113, 13, 0, NULL);
    mymap_mmap(&memMap, (void*)155, 13, 0, NULL);
    mymap_mmap(&memMap, (void*)110, 18, 0, NULL);

    assert_int_equal( mymap_size(&memMap), 10 );

    mymap_munmap(&memMap, (void*)96);

    const size_t ret = mymap_size(&memMap);
    assert_int_equal( ret, 9 );

    assert_int_equal( mymap_isValid(&memMap), 0 );

    mymap_release(&memMap);
}

static void test_mymap_init_NULL(void **state) {
    (void) state; /* unused */

    const int ret = mymap_init(NULL);
    assert_int_equal( ret, -1 );
}

static void test_mymap_init_valid(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    const int ret = mymap_init(&memMap);
    assert_int_equal( ret, 0 );

    assert_int_equal( mymap_isValid(&memMap), 0 );

    mymap_release(&memMap);
}

static void test_mymap_size_NULL(void **state) {
    (void) state; /* unused */

    assert_int_equal( mymap_size(NULL), 0 );
}

static void test_mymap_size_empty(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    memMap.root = NULL;

    assert_int_equal( mymap_size(&memMap), 0 );
}

static void test_mymap_startAddress_NULL(void **state) {
    (void) state; /* unused */

    assert_int_equal( mymap_startAddress(NULL), NULL );
}

static void test_mymap_startAddress_empty(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    memMap.root = NULL;

    assert_int_equal( mymap_startAddress(&memMap), NULL );
}

static void test_mymap_startAddress_normal(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);

    mymap_mmap(&memMap, (void*)10, 10, 0, NULL);

    assert_int_equal( mymap_startAddress(&memMap), 10 );

    mymap_release(&memMap);
}

static void test_mymap_endAddress_NULL(void **state) {
    (void) state; /* unused */

    assert_int_equal( mymap_endAddress(NULL), NULL );
}

static void test_mymap_endAddress_empty(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    memMap.root = NULL;

    assert_int_equal( mymap_endAddress(&memMap), NULL );
}

static void test_mymap_endAddress_normal(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);

    mymap_mmap(&memMap, (void*)10, 10, 0, NULL);

    assert_int_equal( mymap_endAddress(&memMap), 20 );

    mymap_release(&memMap);
}

static void test_mymap_isValid_NULL(void **state) {
    (void) state; /* unused */

    assert_int_equal( mymap_isValid(NULL), 0 );
}

static void test_mymap_isValid_empty(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    memMap.root = NULL;

    assert_int_equal( mymap_isValid(&memMap), -1 );
}

static void test_mymap_release_NULL(void **state) {
    (void) state; /* unused */

    const int ret = mymap_release(NULL);
    assert_int_equal( ret, -1 );
}

static void test_mymap_release_double(void **state) {
    (void) state; /* unused */

    ContainerType memMap;
    mymap_init(&memMap);

    mymap_mmap(&memMap, (void*)10, 10, 0, NULL);

    assert_int_equal( mymap_release(&memMap), 1 );
    assert_int_equal( mymap_release(&memMap), -2 );
}



int main(void) {
    const struct UnitTest tests[] = {
        unit_test(test_mymap_mmap_NULL),
        unit_test(test_mymap_mmap_empty),
        unit_test(test_mymap_mmap_first),
        unit_test(test_mymap_mmap_second),
        unit_test(test_mymap_mmap_segmented_toLeft),
        unit_test(test_mymap_mmap_segmented_toRight),

        unit_test(test_mymap_munmap_NULL),
        unit_test(test_mymap_munmap_empty),
        unit_test(test_mymap_munmap_empty2),
        unit_test(test_mymap_munmap_badaddr),
        unit_test(test_mymap_munmap_root),
        unit_test(test_mymap_munmap_root2),
        unit_test(test_mymap_munmap_right),
        unit_test(test_mymap_munmap_right2),
        unit_test(test_mymap_munmap_left),
        unit_test(test_mymap_munmap_left2),
        unit_test(test_mymap_munmap_subtree),

        unit_test(test_mymap_init_NULL),
        unit_test(test_mymap_init_valid),

        unit_test(test_mymap_size_NULL),
        unit_test(test_mymap_size_empty),
        unit_test(test_mymap_startAddress_NULL),
        unit_test(test_mymap_startAddress_empty),
        unit_test(test_mymap_startAddress_normal),
        unit_test(test_mymap_endAddress_NULL),
        unit_test(test_mymap_endAddress_empty),
        unit_test(test_mymap_endAddress_normal),

        unit_test(test_mymap_isValid_NULL),
        unit_test(test_mymap_isValid_empty),

        unit_test(test_mymap_release_NULL),
        unit_test(test_mymap_release_double),
    };

    return run_group_tests(tests);
}
