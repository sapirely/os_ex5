#include "VirtualMemory.h"
#include "PhysicalMemory.h"

// ------------------------------------- todos & notes ------------------------------------- //
//todo: chane ints to uint64_t
// deleted addressToBin since can do bit operations on any number
// addressInterpreter might be redundant


// ------------------------------------- library methods ------------------------------------- //

void clearTable(uint64_t frameIndex) {
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        PMwrite(frameIndex * PAGE_SIZE + i, 0);
    }
}

void VMinitialize() {
    clearTable(0);
}


int VMread(uint64_t virtualAddress, word_t* value) {
    uint64_t frame = callTraverseTree(virtualAddress);
    uint64_t offset = calcOffset(virtualAddress, OFFSET_WIDTH);
    PMread((frame*PAGE_SIZE)+offset, value);
    // when do we fail
    return 1;
}


int VMwrite(uint64_t virtualAddress, word_t value) {
    uint64_t frame = callTraverseTree(virtualAddress);
    uint64_t offset = calcOffset(virtualAddress, OFFSET_WIDTH);
    PMwrite((frame*PAGE_SIZE)+offset, value);
    // when do we fail
    return 1;
}

// ------------------------------------- inner methods ------------------------------------- //

/**
 * @param address
 * @param offsetSize
 * @return the offset based on the address and the offset size.
 */
uint64_t calcOffset(uint64_t address, uint64_t offsetSize){
    uint64_t powerOfTwo = 2;
    powerOfTwo<<=offsetSize-1; //2^OFFSETWIDTH
    uint64_t pattern = powerOfTwo-1; // A number with OFFSET_WIDTH ones
    return (pattern & address);
}

/**
 *
 * @param address
 * @param index - output
 * @param offset - output
 * @param offsetSize
 */
void addressInterpreter(int address, int* index, int* offset, int offsetSize) // offsetSize = OFFSET_WIDTH
{
    // changed to int - an address is at most 2^20, within int limits
    int binaryAddress = address;
    *offset = calcOffset(binaryAddress, offsetSize);
    binaryAddress >>= offsetSize; //shift right - trims the offset
    *index = binaryAddress;
}


/**
 * Gets an address and returns it split to indices and offset - used to traverse over the tree
 * @param address
 * @param parsedAddress - output
 */
void parseAddress(uint64_t address, uint64_t * parsedAddress){

    // parsed address is an array such as that every cell is the "offset" of a page table
    // the real offset is saved in the last cell
    // the index part of the address is split into TABLE_DEPTH indices, each of them PAGE_SIZE sized
    // so every index is used to navigate in a level of the tree - a page table
//    parsedAddress[TABLES_DEPTH-1] = calcOffset(address, OFFSET_WIDTH);
//    address>>=OFFSET_WIDTH; // trim offset
    for (int i=TABLES_DEPTH-1; i>=0; i--){
        parsedAddress[i] = calcOffset(address, OFFSET_WIDTH);
        address>>=OFFSET_WIDTH; // trim index
    }
}

/**
 * Iterates over tree based on an address: [index, offset].
 * returns a frame. (if 0, finds a free frame)
 * @param address
 * @return frame
 */
uint64_t callTraverseTree(uint64_t address){
    uint64_t parsedAddress[TABLES_DEPTH];
    int index, offset;
    addressInterpreter(address, &index, &offset, OFFSET_WIDTH);
    parseAddress(address, parsedAddress);
    int i=0;
    uint64_t root=0;
    uint64_t maxUsedFrameIdx = 0;
    uint64_t maxDistFrameIndex = 0;
    uint64_t maxDistParent = 0;
    uint64_t maxDistPage = 0;
    double maxDist = 0;

    RecursionContext context = {root, 0, 0, &maxDistPage, &maxDistFrameIndex, &maxDistParent, &maxDist,
                                index, &maxUsedFrameIdx, 0};


    int frame = traverseTree(0, 0, parsedAddress, address, context);
    return (uint64_t) frame; // todo: fix
}

/**
 * INNER FUNC!!! use callTraverseTree. //todo: hide
 * Iterates over tree based on an address: [index, offset].
 * returns a frame. (if 0, finds a free frame)
 * @return
 */

// there are TABLE_DEPTH levels, each table is PAGE_SIZE size;
//
int traverseTree(int depth, int root, uint64_t *parsedAddress, int originalIndex, RecursionContext context){
    int index;
    uint64_t parent;
    parent = root;
    RecursionContext currContext = context;
    // added the following bc they were weirdly large - is it ok? todo
    currContext.rootIndex=0;
    currContext.currDepth=0;
    if (depth<TABLES_DEPTH-1) // -1 bc we don't want the value in the frame, just the frame number
    {
        index = parsedAddress[depth];
        PMread((root * PAGE_SIZE) + index, &root);
        if (root==0){
            currContext.root = root;
            currContext.pageSwapInIdx = originalIndex;

            root = findUnusedFrame(currContext);
            if (root == -1){
                // case: unused frames exist
                if (*(currContext.maxUsedFrameIdx) < NUM_FRAMES-1){
                    root = *(currContext.maxUsedFrameIdx)+1;
                    clearTable(root);
                } else { // all frames are used -> evict
                    swapPage(*(currContext.maxDistPage), *(currContext.maxDistPageIndex), *(currContext.maxDistParent),
                             parent);
                    root = *(currContext.maxDistPage);
                }
            } else { // found unused frame
                clearTable(root);
            }
            currContext.lastTableCreated = root;
            PMwrite((parent * PAGE_SIZE) + index, root);
        }
        depth++;
        return traverseTree(depth, root, parsedAddress, originalIndex, currContext);
    } else { // finished traversing over the tree, last frame is in root
        return root;
    }
}

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
int findUnusedFrame(RecursionContext context)
{
    bool tableIsEmpty = true;
    word_t referencedFrameIdx;
    for (int entry_idx = 0; entry_idx < PAGE_SIZE; entry_idx++)
    {
        PMread((context.root * PAGE_SIZE) + entry_idx, &referencedFrameIdx);
        if (referencedFrameIdx > *(context.maxUsedFrameIdx)) {
            // update max referenced frame
            *(context.maxUsedFrameIdx) = (uint64_t)referencedFrameIdx;
        }
        if (referencedFrameIdx != 0) {
            tableIsEmpty = false;
            if (context.currDepth == TABLES_DEPTH) {
                // we've reached a leaf, i.e a reference to a page
                double cyclicDist = calcCyclicDist((uint64_t)referencedFrameIdx, context.pageSwapInIdx);
                if (cyclicDist > *(context.maxDist)) {
                    *(context.maxDist) = cyclicDist;
                    *(context.maxDistPage) = (uint64_t)referencedFrameIdx;
                    *(context.maxDistParent) = context.root;
                }
                return -1; // no empty table was found
            } else {
                // not a leaf - check children.
                context.rootIndex<<=OFFSET_WIDTH;
                context.rootIndex += referencedFrameIdx;
                context.currDepth++;
                uint64_t parent = context.root;
                context.root = referencedFrameIdx;
                int retVal = findUnusedFrame(context);
                context.root = parent;
                context.rootIndex -= referencedFrameIdx;
                context.rootIndex>>=OFFSET_WIDTH;
                context.currDepth--;
                if (retVal == -1) {
                    // no empty table was found in the child and his children
                    continue;
                } else if (retVal == 0) {
                    // the child is an empty table
                    PMwrite((context.root * PAGE_SIZE) + entry_idx ,0); // remove reference to table
                    return referencedFrameIdx;
                } else {
                    // found an empty table in grandson or one of his descendants.
                    return retVal;
                }
            }
        }
    }
    if (tableIsEmpty && (context.lastTableCreated != context.rootIndex)) {
        // we ran through all entries, which are 0, and it's not a table we've just created
        return 0;
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

/**
 * called if findUnusedFrame fails.
 * clear table and delete parent reference to it
 */
void swapPage(uint64_t& frameIndex, uint64_t& pageIndex, uint64_t frameParent, uint64_t newParentFrame)
{
    PMevict(frameIndex, pageIndex);
    PMwrite(frameParent ,0);
    clearTable(frameIndex);
//    PMwrite(frameIndex,newPageIndex);
    PMwrite(newParentFrame,frameIndex);
}