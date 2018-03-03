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

#include "memorymap/MemoryArea.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>



static void memory_fitBetween_NULL(void **state) {
    (void) state; /* unused */

    const int ret = memory_fitBetween(NULL, NULL, NULL);
    assert_int_equal( ret, -1 );
}

static void memory_fitBetween_NULL_range(void **state) {
    (void) state; /* unused */

    MemoryArea check;
    check.offset = 50;
    check.size = 10;

    const int ret = memory_fitBetween(NULL, NULL, &check);
    assert_int_equal( ret, 0 );
    assert_int_equal( check.offset, 50 );
}

static void memory_fitBetween_first_NULL(void **state) {
    (void) state; /* unused */

    MemoryArea second;
    second.offset = 100;
    second.size = 100;

    MemoryArea check;
    check.offset = 50;
    check.size = 10;

    const int ret = memory_fitBetween(NULL, &second, &check);
    assert_int_equal( ret, 0 );
    assert_int_equal( check.offset, 50 );
}

static void memory_fitBetween_first_NULL_fail(void **state) {
    (void) state; /* unused */

    MemoryArea second;
    second.offset = 100;
    second.size = 100;

    MemoryArea check;
    check.offset = 150;
    check.size = 10;

    const int ret = memory_fitBetween(NULL, &second, &check);
    assert_int_equal( ret, -1 );
}

static void memory_fitBetween_second_NULL(void **state) {
    (void) state; /* unused */

    MemoryArea first;
    first.offset = 20;
    first.size = 100;

    MemoryArea check;
    check.offset = 100;
    check.size = 10;

    const int ret = memory_fitBetween(&first, NULL, &check);
    assert_int_equal( ret, 0 );
    assert_int_equal( check.offset, 120 );
}

static void memory_fitBetween_second_NULL_moved(void **state) {
    (void) state; /* unused */

    MemoryArea first;
    first.offset = 100;
    first.size = 100;

    MemoryArea check;
    check.offset = 50;
    check.size = 10;

    const int ret = memory_fitBetween(&first, NULL, &check);
    assert_int_equal( ret, 0 );
    assert_int_equal( check.offset, 200 );
}



int main(void) {
    const struct UnitTest tests[] = {
        unit_test(memory_fitBetween_NULL),
        unit_test(memory_fitBetween_NULL_range),
        unit_test(memory_fitBetween_first_NULL),
        unit_test(memory_fitBetween_first_NULL_fail),
        unit_test(memory_fitBetween_second_NULL),
        unit_test(memory_fitBetween_second_NULL_moved),
    };

    return run_group_tests(tests);
}
