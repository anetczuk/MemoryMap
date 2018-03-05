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

#include "memorymap/LinkedList.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>


static void list_mmap_NULL(void **state) {
    (void) state; /* unused */

    const void* ret = list_mmap(NULL, NULL, 0);

    assert_null( ret );
}

static void list_mmap_first(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);

    const void* ret = list_mmap(&list, (void*)128, 64);
    assert_int_equal( ret, 128 );

    list_release(&list);
}

static void list_mmap_second(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);

    list_mmap(&list, (void*)160, 64);

    const void* ret = list_mmap(&list, (void*)128, 64);
    assert_int_equal( ret, 224 );

    list_release(&list);
}

static void list_mmap_segmented(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);

    list_mmap(&list, (void*)100, 64);
    /// small space between segments
    list_mmap(&list, (void*)200, 64);

    const void* ret = list_mmap(&list, (void*)128, 64);
    assert_int_equal( ret, 264 );

    list_release(&list);
}

static void list_munmap_NULL(void **state) {
    (void) state; /* unused */

    list_munmap(NULL, NULL);

    assert_true( 1 );
}

static void list_munmap_empty(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);

    list_munmap(&list, NULL);

    list_release(&list);
}

static void list_munmap_badaddr(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);
    list_mmap(&list, (void*)100, 64);

    list_munmap(&list, (void*)50);

    const size_t ret = list_size(&list);
    assert_int_equal( ret, 1 );

    list_release(&list);
}

static void list_munmap_first(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);

    list_mmap(&list, (void*)100, 64);
    list_mmap(&list, (void*)200, 64);

    list_munmap(&list, (void*)120);

    const size_t ret = list_size(&list);
    assert_int_equal( ret, 1 );

    list_release(&list);
}

static void list_munmap_second(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);

    list_mmap(&list, (void*)100, 64);
    list_mmap(&list, (void*)200, 64);
    list_mmap(&list, (void*)300, 64);

    list_munmap(&list, (void*)220);

    const size_t ret = list_size(&list);
    assert_int_equal( ret, 2 );

    list_release(&list);
}

static void list_munmap_between(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);

    list_mmap(&list, (void*)100, 64);
    list_mmap(&list, (void*)200, 64);
    list_mmap(&list, (void*)300, 64);

    list_munmap(&list, (void*)290);

    const size_t ret = list_size(&list);
    assert_int_equal( ret, 3 );

    list_release(&list);
}

static void list_init_NULL(void **state) {
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


static void list_release_NULL(void **state) {
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

    list_add(&list, 1, 1);
    list_add(&list, 2, 1);

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

static void list_add_NULL(void **state) {
    list_add(NULL, 3, 1);
}

static void list_add_first(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);
    list_add(&list, 3, 1);
    list_add(&list, 1, 1);

    const size_t lSize = list_size(&list);
    assert_int_equal( lSize, 2 );

    const MemoryArea* mem = list_get(&list, 0);
    assert_non_null( mem );
    assert_int_equal( mem->start, 1 );

    list_release(&list);
}

static void list_add_last(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);
    list_add(&list, 1, 1);
    list_add(&list, 5, 1);

    const size_t lSize = list_size(&list);
    assert_int_equal( lSize, 2 );

    const MemoryArea* mem = list_get(&list, 1);
    assert_non_null( mem );
    assert_int_equal( mem->start, 5 );

    list_release(&list);
}

static void list_add_middle(void **state) {
    (void) state; /* unused */

    LinkedList list;
    list_init(&list);
    list_add(&list, 1, 1);
    list_add(&list, 5, 1);
    list_add(&list, 3, 1);

    const size_t lSize = list_size(&list);
    assert_int_equal( lSize, 3 );

    const MemoryArea* mem = list_get(&list, 1);
    assert_non_null( mem );
    assert_int_equal( mem->start, 3 );

    list_release(&list);
}


/// ==================================================


int main(void) {
    const struct UnitTest tests[] = {
        unit_test(list_release_NULL),
        unit_test(list_release_list),
        unit_test(list_release_2),
        unit_test(list_add_NULL),
        unit_test(list_add_first),
        unit_test(list_add_middle),
        unit_test(list_add_last),
        unit_test(list_size_NULL),
        unit_test(list_size_0),

        unit_test(list_mmap_NULL),
        unit_test(list_mmap_first),
        unit_test(list_mmap_second),
        unit_test(list_mmap_segmented),

        unit_test(list_munmap_NULL),
        unit_test(list_munmap_empty),
        unit_test(list_munmap_badaddr),
        unit_test(list_munmap_first),
        unit_test(list_munmap_second),
        unit_test(list_munmap_between),

        unit_test(list_init_NULL),
        unit_test(list_init_valid)
    };

    return run_group_tests(tests);
}
