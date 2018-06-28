#pragma once

// ------------------------------------- includes ------------------------------------- //
//#include <string>
#include "MemoryConstants.h"
#include <cstdio>
#include <bitset>
#include <algorithm>
#include <cmath>

#define INDEX_LENGTH (VIRTUAL_ADDRESS_WIDTH-OFFSET_WIDTH) // is this correct todo

// ------------------------------------- RecursionContext struct------------------------------------- //

typedef struct
{
    uint64_t root;
    uint64_t rootIndex;
    uint64_t parent;
    int currDepth;
    uint64_t* maxDistPage;
    uint64_t* maxDistPageIndex;
    uint64_t* maxDistParent;
    double* maxDist;
    uint64_t pageSwapInIdx;
    uint64_t* maxUsedFrameIdx;
    uint64_t lastTableCreated;

}RecursionContext;


// ------------------------------------- library methods ------------------------------------- //
/*
 * Initialize the virtual memory
 */
void VMinitialize();

/* reads a word from the given virtual address
 * and puts its content in *value.
 *
 * returns 1 on success.
 * returns 0 on failure (if the address cannot be mapped to a physical
 * address for any reason)
 */
int VMread(uint64_t virtualAddress, word_t* value);

/* writes a word to the given virtual address
 *
 * returns 1 on success.
 * returns 0 on failure (if the address cannot be mapped to a physical
 * address for any reason)
 */

int VMwrite(uint64_t virtualAddress, word_t value);


// ------------------------------------- inner methods ------------------------------------- //


/**
 * Translates a decimal address to binary.
 * @param address
 * @return
 */
std::bitset<VIRTUAL_ADDRESS_WIDTH> addressToBin(int address);

/**
 * Interpret a binary address as an index + offset.
 * @param address
 * @param addressTuple - input, function updates it to [index, offset]
 */
void addressInterpreter(int address, int* index, int* offset, int offsetSize);

/**
 * Find page with max cyclic distance from the target page.
 * @param targetPage
 * @return index of page with max cyclic distance from the target page.
 */
int maxCyclicDist(int targetPage);
// no need - it is calculated in findUnusedFrame()


/**
 * @param root root of the subtree (frame).
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
int findUnusedFrame(RecursionContext context);

/**
 * called if findUnusedFrame fails.
 * use maxCyclicDist, clear table, and delete parent reference to it
 *
 */
void swapPage(uint64_t& oldFrameIndex, uint64_t& oldPageIndex, uint64_t oldFrameParent, uint64_t newParentFrame);

/**
 * @param address
 * @param offsetSize
 * @return the offset based on the address and the offset size.
 */
uint64_t calcOffset(uint64_t address, uint64_t offsetSize);

/**
 * Gets an address and returns it split to indices and offset - used to traverse over the tree
 * @param address
 * @param parsedAddress - output
 */
void parseAddress(uint64_t address, uint64_t * parsedAddress);

/**
 * Iterates over tree based on an address: [index, offset].
 * returns a frame. (if 0, finds a free frame)
 * @param address
 * @return frame
 */
uint64_t callTraverseTree(uint64_t address);


/**
 * INNER FUNC!!! use callTraverseTree.
 * Iterates over tree based on an address: [index, offset].
 * returns a frame. (if 0, finds a free frame)
 * @return
 */
int traverseTree(int depth, int root, uint64_t *parsedAddress, int originalIndex, RecursionContext context);


/**
 * Calculates the cyclic distance of the page in index pageIdx from the index of the page that we
 * want to retrieve.
 * @return
 */
double calcCyclicDist(uint64_t pageIdx, uint64_t pageSwapInIdx);