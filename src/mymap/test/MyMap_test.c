/*
 * MyMap_test.c
 *
 *  Created on: Mar 1, 2018
 *      Author: bob
 */

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
