#pragma once

// ------------------------------------- includes ------------------------------------- //
//#include <string>
#include "MemoryConstants.h"
#include <cstdio>

// ------------------------------------- PT in the tree ------------------------------------- //
struct Table
{
    int* table;
    int maxReferencedFrame;
}Table;


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
int* addressToBin(int address);

/**
 * Interpret a binary address as an index + offset.
 * @param address
 * @return [index, offset] array of ints
 */
int* addressInterpreter(int address);

/**
 * Find page with max cyclic distance from the target page.
 * @param targetPage
 * @return index of page with max cyclic distance from the target page.
 */
int maxCyclicDist(int targetPage);

/**
 * @return idx of an unused frame in the physical memory (if an empty table was found, the method
 * initializes its entries to 0 and returns an idx of a frame in the table).
 */
int findUnusedFrame();

/**
 * called if findUnusedFrame fails.
 * use maxCyclicDist, clear table, and delete parent reference to it
 * @return
 */
int swapPage();

/**
 * Iterates over tree based on an address: [index, offset].
 * returns a frame. (if 0, finds a free frame)
 * @param tree
 * @return
 */
int* traverseTree(int **tree, int* address);

/**
 * calculates the depth of the tree.
 * @param virtualMemorySize
 * @param pageSize
 * @return
 */
int findTreeDepth(int virtualMemorySize, int pageSize);

/**
 * After traversing over the tree and finding the frame, write to it.
 * @return
 */
int writeToLeaf(int frameNum, int pageNum);

/**
 * After traversing over the tree and finding the frame, read from it.
 * @return
 */
int readFromLeaf(int frameNum, int pageNum);