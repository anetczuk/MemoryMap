## MemoryMap

Implementation of memory map based on red-black trees. Main role of memory map is memory blocks arrangement allowing fast addition or removal of blocks.

Map consists of two primary operations:
* _reserve_ of new memory block described by start address and block size
* _remove_ of existing block by any address within range of the block

In case of memory segmentation - new block cannot be added in pointed address - new block is reserved at first available place starting from desired address.  


### Features

* red-black trees implementation without 3rd party dependencies
* unit tests using cmocka library
* valgrind integration
* code coverage calculation
* cppcheck analysis


### Modules

* _memorymap/LinkedList.h_ contains implementation of memory map based on linked list
* _memorymap/RBTree.h_ implementation of memory map based on red-black trees
* _mymap/MyMap.h_ access interface to memory map using _RBTree.h_ under the hood


### Examples

* _mymap/example/main.c_ use example of MyMap.h
* _memorymap/test/*.c_ unit tests of _memorymap_ module
* _mymap/test/*.c_ unit tests of _mymap_ module


### Requirements

* C99 standard
* cmocka (libcmocka-dev) library - for unit tests
* gcc - for code coverage
* valgrind - for memory leaks checking
* cppcheck - for static code analysis


### Usage

Run following commands inside build directory. Building in source is forbidden.

Project configuration: 

_cmake {path to src dir}_

for code coverage run following: 

_cmake -DCODE_COVERAGE=ON {path to src dir}_

Compilation:

_make_

Running tests:

_ctest_

Checking memory leaks on tests, eg. on _RBTree.h_:

_./memorymap/test/test_RBTree_test.sh --tool=memcheck_

Running callgrind on tests, eg. on _RBTree.h_:

_./memorymap/test/test_RBTree_test.sh --tool=callgrind_

Measuring code coverage of tests, eg. for _RBTree.h_:

_./memorymap/test/test_RBTree_test.sh --tool=coverage_

Running example program:

_./mymap/example/mymap_example_


Static check of code (cppcheck):

_{src dir}/staticcheck.sh_

optional configuration checking:

_{src dir}/staticcheck.sh -cc_


### ToDo

* integrate cppcheck
* add clang analyzer
* compare with other libraries/implementations
* add selective run of test cases based on it's names


### References

* [Memory segmentation](https://en.wikipedia.org/wiki/Memory_segmentation)
* [Memory Map](http://www.embeddedc.in/p/automotive-basics-part5.html)
* [Red-black trees](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree)
