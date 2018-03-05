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


static void memory_size_NULL(void **state) {
    (void) state; /* unused */

	const size_t ret = memory_size(NULL);
	assert_int_equal( ret, 0 );
}

static void memory_size_valid(void **state) {
    (void) state; /* unused */

	const MemoryArea check = memory_create(50, 10);

	const size_t ret = memory_size(&check);
	assert_int_equal( ret, 10 );
}

static void memory_fitBetween_NULL(void **state) {
    (void) state; /* unused */

    const int ret = memory_fitBetween(NULL, NULL, NULL);
    assert_int_equal( ret, -1 );
}

static void memory_fitBetween_NULL_range(void **state) {
    (void) state; /* unused */

    MemoryArea check = memory_create(50, 10);

    const int ret = memory_fitBetween(NULL, NULL, &check);
    assert_int_equal( ret, 0 );
    assert_int_equal( check.start, 50 );
    assert_int_equal( check.end, 60 );
}

static void memory_fitBetween_first_NULL(void **state) {
    (void) state; /* unused */

    const MemoryArea second = memory_create(100, 100);

    MemoryArea check = memory_create(50, 10);

    const int ret = memory_fitBetween(NULL, &second, &check);
    assert_int_equal( ret, 0 );
    assert_int_equal( check.start, 50 );
    assert_int_equal( check.end, 60 );
}

static void memory_fitBetween_first_NULL_fail(void **state) {
    (void) state; /* unused */

    const MemoryArea second = memory_create(100, 200);

    MemoryArea check = memory_create(150, 10);

    const int ret = memory_fitBetween(NULL, &second, &check);
    assert_int_equal( ret, -1 );
}

static void memory_fitBetween_second_NULL(void **state) {
    (void) state; /* unused */

    const MemoryArea first = memory_create(20, 100);

    MemoryArea check = memory_create(100, 10);

    const int ret = memory_fitBetween(&first, NULL, &check);
    assert_int_equal( ret, 0 );
    assert_int_equal( check.start, 120 );
    assert_int_equal( check.end, 130 );
}

static void memory_fitBetween_second_NULL_moved(void **state) {
    (void) state; /* unused */

    const MemoryArea first = memory_create(100, 100);

    MemoryArea check = memory_create(50, 10);

    const int ret = memory_fitBetween(&first, NULL, &check);
    assert_int_equal( ret, 0 );
    assert_int_equal( check.start, 200 );
    assert_int_equal( check.end, 210 );
}

static void memory_fitBetween_between_exact(void **state) {
    (void) state; /* unused */

    const MemoryArea first  = memory_create(100, 20);
    const MemoryArea second = memory_create(200, 20);

    MemoryArea check = memory_create(150, 10);

    const int ret = memory_fitBetween(&first, &second, &check);
    assert_int_equal( ret, 0 );
    assert_int_equal( check.start, 150 );
    assert_int_equal( check.end, 160 );
}

static void memory_fitBetween_between_moved(void **state) {
    (void) state; /* unused */

    const MemoryArea first  = memory_create(100, 20);
    const MemoryArea second = memory_create(200, 20);

    MemoryArea check = memory_create(110, 10);

    const int ret = memory_fitBetween(&first, &second, &check);
    assert_int_equal( ret, 0 );
    assert_int_equal( check.start, 120 );
    assert_int_equal( check.end, 130 );
}

static void memory_fitBetween_between_nospace(void **state) {
    (void) state; /* unused */

    const MemoryArea first  = memory_create(100, 20);
    const MemoryArea second = memory_create(200, 20);

    MemoryArea check = memory_create(110, 85);

    const int ret = memory_fitBetween(&first, &second, &check);
    assert_int_equal( ret, -1 );
}

static void memory_fitAfter_NULL(void **state) {
    (void) state; /* unused */

    MemoryArea check = memory_create(50, 10);

    memory_fitAfter(NULL, &check);
    assert_int_equal( check.start, 50 );
    assert_int_equal( check.end, 60 );
}

static void memory_fitAfter_before(void **state) {
    (void) state; /* unused */

    const MemoryArea segment = memory_create(100, 100);

    MemoryArea check = memory_create(50, 10);

    memory_fitAfter(&segment, &check);
    assert_int_equal( check.start, 200 );
    assert_int_equal( check.end, 210 );
}

static void memory_fitAfter_inside(void **state) {
    (void) state; /* unused */

    const MemoryArea segment = memory_create(100, 100);

    MemoryArea check = memory_create(150, 10);

    memory_fitAfter(&segment, &check);
    assert_int_equal( check.start, 200 );
    assert_int_equal( check.end, 210 );
}

static void memory_fitAfter_after(void **state) {
    (void) state; /* unused */

    const MemoryArea segment = memory_create(20, 100);

    MemoryArea check = memory_create(200, 10);

    memory_fitAfter(&segment, &check);
    assert_int_equal( check.start, 200 );
    assert_int_equal( check.end, 210 );
}




int main(void) {
    const struct UnitTest tests[] = {
        unit_test(memory_size_NULL),
        unit_test(memory_size_valid),
        unit_test(memory_fitBetween_NULL),
        unit_test(memory_fitBetween_NULL_range),
        unit_test(memory_fitBetween_first_NULL),
        unit_test(memory_fitBetween_first_NULL_fail),
        unit_test(memory_fitBetween_second_NULL),
        unit_test(memory_fitBetween_second_NULL_moved),
        unit_test(memory_fitBetween_between_exact),
        unit_test(memory_fitBetween_between_moved),
        unit_test(memory_fitBetween_between_nospace),

        unit_test(memory_fitAfter_NULL),
        unit_test(memory_fitAfter_before),
        unit_test(memory_fitAfter_inside),
        unit_test(memory_fitAfter_after),
    };

    return run_group_tests(tests);
}
