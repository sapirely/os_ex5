#include "VirtualMemory.h"

#include <cstdio>
#include <cassert>

int main(int argc, char **argv) {
//    uint64_t a=addressToBin(31);
    int a[2];
    unsigned int b=31;
    b>>=1;
    printf("b:%d", b);
    int index, offset;

//    addressInterpreter(31,&index, &offset, OFFSET_WIDTH);
    int num = 1;
    num = (num>>=1);
    printf("num: %d",PAGE_SIZE);
    uint64_t arr[TABLES_DEPTH];
    parseAddress(1194537, arr);


//    VMinitialize();
//    for (uint64_t i = 0; i < (2 * NUM_FRAMES); ++i) {
//        printf("writing to %llu\n", (long long int) i);
//        VMwrite(5 * i * PAGE_SIZE, i);
//    }
//
//    for (uint64_t i = 0; i < (2 * NUM_FRAMES); ++i) {
//        word_t value;
//        VMread(5 * i * PAGE_SIZE, &value);
//        printf("reading from %llu %d\n", (long long int) i, value);
//        assert(uint64_t(value) == i);
//    }
//    printf("success\n");

    return 0;
}