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

#include <time.h>               /// time
#include <stdlib.h>             /// srand
#include <stdio.h>              /// printf


int main(void) {

    /// initializing random number generator
    const unsigned int seed = time(NULL);
    srand( seed );
    printf( "Seed: %u\n", seed );

    /// initialize the container
    map_t memoryMap;
    mymap_init(&memoryMap);

    /// allocating memory
    printf( "\nAcquiring memory:\n" );
    for(size_t i=0; i<16; ++i) {
        void* addr = (void*) (unsigned long) (rand() % 512 +1);
        const size_t msize = rand() % 64 +1;
        void* receivedAddress = mymap_mmap(&memoryMap, addr, msize, 0, NULL);
        printf("Trying to reserve memory block 0x%02lx at address %p, received address: %p\n", msize, addr, receivedAddress);
    }

    printf( "\nMemory structure:\n" );
    mymap_dump(&memoryMap);

    const void* startAddress = mymap_startAddress(&memoryMap);
    const void* endAddress = mymap_endAddress(&memoryMap);
    const unsigned long memorySize = (unsigned long)endAddress - (unsigned long)startAddress;
    printf( "\nTotal memory range: %p - %p, size: 0x%lx\n", startAddress, endAddress, memorySize );

    /// removing memory segment
    void* removeAddress = (void*) (rand() % memorySize + (unsigned long)startAddress);
    printf( "Selected random segment to remove: %p\n", removeAddress );

    const size_t sizeBefore = mymap_size(&memoryMap);

    mymap_munmap(&memoryMap, removeAddress);

    const size_t sizeAfter = mymap_size(&memoryMap);

    if (sizeAfter < sizeBefore) {
        printf( "Memory block found and removed\n");
    } else {
        printf( "Memory block not found\n");
    }

    printf( "\nMemory structure after removal:\n" );
    mymap_dump(&memoryMap);

    /// release resources
    mymap_release(&memoryMap);

    return 0;
}
