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


static void mymap_mmap_param_NULL(void **state) {
    (void) state; /* unused */

    const void* ret = mymap_mmap(NULL, NULL, 0, 0, NULL);

    assert_null( ret );
}

static void mymap_munmap_param_NULL(void **state) {
    (void) state; /* unused */

    mymap_munmap(NULL, NULL);

    assert_true( 1 );
}

static void mymap_init_param_NULL(void **state) {
    (void) state; /* unused */

    const int ret = mymap_init(NULL);

    assert_int_equal( ret, -1 );
}

static void mymap_dump_param_NULL(void **state) {
    (void) state; /* unused */

    const int ret = mymap_dump(NULL);

    assert_int_equal( ret, -1 );
}



int main(void) {
    const struct UnitTest tests[] = {
        unit_test(mymap_mmap_param_NULL),
        unit_test(mymap_munmap_param_NULL),
        unit_test(mymap_init_param_NULL),
        unit_test(mymap_dump_param_NULL),
    };

    return run_group_tests(tests);
}
