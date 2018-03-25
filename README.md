## MemoryMap

Implementation of memory map based on red-black trees. Main role of memory map is memory blocks arrangement allowing fast addition or removal of blocks.

Map consists of two primary operations:
* _reserve_ of new memory block described by start address and block size
* _remove_ of existing block by any address within range of the block

In case of memory segmentation - new block cannot be added in pointed address - new block is reserved at first available place starting from desired address.  


### Features

* generalised red-black trees implementation without 3rd party dependencies
* unit tests using cmocka library
* valgrind integration
* code coverage calculation (more than 95% of code covered by tests)
* cppcheck analysis
* clang static analysis


### Generalised red-black tree

Core part is implemented in _rbtree/AbstractRBTree.h_. It consists of generalisation based on _void*_ and function pointers. Those pointers enables storing any data in the tree, by simply providing implementation of following operations:
* _less_ comparison operator
* _can insert right_ - checking if value can be inserted into _right_ branch of node
* _can insert left_ - checking if value can be inserted into _left_ branch of node
* _try fit right_ - generalised fitting value into _right_ branch of node
* _try fit left_ - generalised fitting value into _left_ branch of node
* _print_ function of stored value
* _delete_ function of stored value


### Modules

* _rbtree/AbstractRBTree.h_ contains abstract(template-like) implementation of red-black trees
* _rbtree/UIntRBTree.h_ contains red-black tree of integers -- use example of _AbstractRBTree_
* _memorymap/LinkedList.h_ contains implementation of memory map based on linked list
* _memorymap/RBTree.h_ implementation of memory map based on red-black trees
* _memorymap/RBTreeV2.h_ implementation of memory map based on _AbstractRBTree_
* _mymap/MyMap.h_ access interface to memory map using _RBTree.h_ under the hood


### Examples

* _mymap/example/main.c_ use example of MyMap.h
* _rbtree/test/*.c_ unit tests of _rbtree_ module
* _memorymap/test/*.c_ unit tests of _memorymap_ module
* _mymap/test/*.c_ unit tests of _mymap_ module


### Requirements

* C99 standard
* cmocka (libcmocka-dev) library - for unit tests
* gcc - for code coverage
* valgrind - for memory leaks checking
* cppcheck - for static code analysis
* clang - for clang analysis tools


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

There is possibility to run all test suites at once:

*./run_tests.sh*

script supports following tools: _memcheck_, _callgrind_, _coverage_


Running example program:

_./mymap/example/mymap_example_


Static check of code (cppcheck):

_{src dir}/staticcheck.sh_

optional configuration checking:

_{src dir}/staticcheck.sh -cc_


To run clang static analysis create separate build folder, go there and run: 

_{src dir}/clanganalyzer.sh_

To activate address sanitizer run following (requires clang):

_cmake -DADDRESS_SANITIZER=ON {path to src dir}_


### ToDo

* compare with other libraries/implementations
* add selective run of test cases based on it's names


### References

* [Memory segmentation](https://en.wikipedia.org/wiki/Memory_segmentation)
* [Memory Map](http://www.embeddedc.in/p/automotive-basics-part5.html)
* [Red-black trees](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree)
