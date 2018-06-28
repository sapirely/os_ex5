
#include <cstdio>
#include <assert.h>
#include <iostream>
#include "VirtualMemory.h"
#include "PhysicalMemory.h"

void printPhysicalMemory()
{
    word_t value;
    for (auto i = 0; i < NUM_FRAMES; i++)
    {
        std::cout << "FRAME: " << i << "\n";
        for (auto j = 0; j < PAGE_SIZE; j++)
        {
            PMread(i * PAGE_SIZE + j, &value);
            std::cout << "  OFFSET " << j << ": " << value << "\n";
        }
        std::cout << "\n";
    }
}

int main()
{


    VMinitialize();


//	printf("NUM_FRAMES: %lli\n",NUM_FRAMES);
//	printf("PAGE_SIZE: %lli\n",PAGE_SIZE);
//	printf("NUM_PAGES: %lli\n",NUM_PAGES);
//	printf("RAM_SIZE: %lli\n\n",RAM_SIZE);

//    std::cout << "PAGE_SIZE: " << PAGE_SIZE << "\nRAM_SIZE: " << RAM_SIZE << "\nNUM_FRAMES: " <<
//              NUM_FRAMES << "\nNUM_PAGES: " << NUM_PAGES << "\n";
    std::cout << "WRITING 98 TO 13\n";
    VMwrite(13, 98);
    word_t value;
    std::cout << "READING FROM 13\n";
    VMread(13, &value);
    std::cout << "### value 1: " << value << "\n";
    std::cout << "WRITING 105 TO 6\n";


    VMwrite(6, 105);
    std::cout << "READING FROM 6\n";
    VMread(6, &value);
    std::cout << "### value 2: " << value << "\n";
    std::cout << "WRITING 51 TO 31\n";



    VMwrite(31, 51);
    std::cout << "READING FROM 31\n";
    VMread(31, &value);
    std::cout << "### value 3: " << value << "\n";


    printPhysicalMemory();

//	VMwrite(31, 82);
//	VMread(31, &value);
//	std::cout << "value 4: " << value << "\n";
//    std::cout << "READING FROM 31\n";
//    std::cout << "### value 4: " << value << "\n";
//
//    VMread(13, &value);
//    VMinitialize();

    return 0;

}