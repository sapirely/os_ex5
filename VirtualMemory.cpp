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

/**
 * @param root root of the subtree.
 * @param maxDistPage idx of the page with max cyclic distance so far
 * @param maxDist max cyclic distance found.
 * @param maxUsedFrameIdx maximal index of a referenced frame.
 * @param currDepth depth of the subtree rooted at currTableAddress.
 * @param maxDistParent parent of the page with max cyclic distance so far.
 * @return idx of an unused frame in the physical memory if an empty table was found (the reference
 * to the table is removed from the tree).
 * Otherwise, the method returns -1 and sets maxUsedFrameIdx to hold the maximal index of a
 * referenced frame, and maxDistPage to hold the index of the page with max cyclic distance.
 */
int findUnusedFrame(uint64_t root, int currDepth, uint64_t& maxDistPage, uint64_t& maxDistParent,
                    uint64_t& maxDist, uint64_t pageSwapInIdx, uint64_t& maxUsedFrameIdx)
{
    word_t referencedFrameIdx;
    for (int entry_idx = 0; entry_idx < PAGE_SIZE; entry_idx++)
    {
        PMread((root * PAGE_SIZE) + entry_idx, &referencedFrameIdx);
        if (referencedFrameIdx > maxUsedFrameIdx) {
            // update max referenced frame
            maxUsedFrameIdx = (uint64_t)referencedFrameIdx;
        }
        if (referencedFrameIdx == 0) {
            // empty entry (page fault)
            if ((entry_idx == 0)) {
                return 0; // signal that an empty table was found
            } else {
                // non-existing table
                return -1; // (we know that all remaining entries are 0 as well)
            }
        } else {
            if (currDepth == TABLES_DEPTH) {
                // we've reached a leaf, i.e a reference to a page
                uint64_t cyclicDist = calcCyclicDist((uint64_t)referencedFrameIdx, pageSwapInIdx);
                if (cyclicDist > maxDist) {
                    maxDist = cyclicDist;
                    maxDistPage = (uint64_t)referencedFrameIdx;
                    maxDistParent = root;
                }
                return -1; // no empty table was found
            } else {
                // not a leaf - check children.
                int retVal = findUnusedFrame((uint64_t)referencedFrameIdx, currDepth+1,
                                             maxDistPage, maxDistParent, maxDist, pageSwapInIdx,
                                             maxUsedFrameIdx);
                if (retVal == -1) {
                    // no empty table was found in the child and his children
                    continue;
                } else if (retVal == 0) {
                    // the child is an empty table
                    PMwrite((root * PAGE_SIZE) + entry_idx ,0); // remove reference to table
                    return referencedFrameIdx;
                } else {
                    // found an empty table in grandson or one of his descendants.
                    return retVal;
                }
            }
        }
    }
    return -1; // no empty table was found.
}


/**
 * Calculates the cyclic distance of the page in index pageIdx from the index of the page that we
 * want to retrieve.
 * @return
 */
double calcCyclicDist(uint64_t pageIdx, uint64_t pageSwapInIdx)
{
    return fmin(NUM_PAGES - abs((int)(pageSwapInIdx - pageIdx)),
                abs((int)(pageSwapInIdx - pageIdx)));
}