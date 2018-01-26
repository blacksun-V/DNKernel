#ifndef PHY_MEM_H
#define PHY_MEM_H

void initPhysicalMemoryManagement(unsigned int memory_size);
void initFreedMemoryRegion(unsigned int base_address, unsigned int size);
void initAllocatedMemoryRegion(unsigned int base_address, unsigned int size);
unsigned int allocSingleMemoryBlock(void);
void freeSingleMemoryBlock(void *physical_address);
void printFreeBlocks();
void printAllocatedBlocks();
void printSystemBlocks();
unsigned int testAddress(unsigned int address);

#endif
