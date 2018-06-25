#include "VirtualMemory.h"
#include "PhysicalMemory.h"



void clearTable(uint64_t frameIndex) {
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        PMwrite(frameIndex * PAGE_SIZE + i, 0);
    }
}

void VMinitialize() {
    clearTable(0);
}


int VMread(uint64_t virtualAddress, word_t* value) {
    return 1;
}


int VMwrite(uint64_t virtualAddress, word_t value) {
    return 1;
}

// ------------------------------------- inner methods ------------------------------------- //

/**
 * Translates a decimal address to binary.
 * @param address
 * @return
 */
std::bitset<VIRTUAL_ADDRESS_WIDTH> addressToBin(int address){
    return std::bitset<VIRTUAL_ADDRESS_WIDTH>(address);
}

/**
 * Interpret a binary address as an index + offset.
 * @param address
 * @param addressTuple - input, function updates it to [index, offset]
 */
void addressInterpreter(int address, uint64_t addressTuple[2])
{
//    uint64_t *addressTuple;
    std::bitset<VIRTUAL_ADDRESS_WIDTH> binaryAddress = addressToBin(address);
    std::bitset<OFFSET_WIDTH> offset;
    std::bitset<INDEX_LENGTH> index;
    index.set(); // sets index bits to ones
    for (int i = 0; i < OFFSET_WIDTH; i++)
    {
        offset.set(i, binaryAddress[0]);
        binaryAddress >>= 1; //shift right - trims the offset
    }
    addressTuple[1] = offset.to_ulong();
    addressTuple[0] = binaryAddress.to_ulong();
}

/**
 * Find page with max cyclic distance from the target page.
 * @param targetPage
 * @return index of page with max cyclic distance from the target page.
 */
int maxCyclicDist(int targetPage){
    int p; // p are the pages already occupied in the table - todo
    ulong maxDist = std::min((ulong)NUM_PAGES-std::abs(targetPage-p),  (ulong) std::abs(targetPage-p));
    // iterate over all p's and find maximal of those (max of mins)
}