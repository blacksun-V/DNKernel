//このファイルは物理メモリを直接管理するためのもの.
//ページングの仮想アドレスに割り当てる4kb=1pageをとったりするやつ

#include <stdint.h>
#define	DEF_MEMORY_KERNEL_START	0x00100000
#define DEF_MEMORY_BLOCK_SIZE 4096
extern void printf (const char *format, ...);

inline unsigned int getSizeOfKernel(void)
{
  unsigned int bss_end, text_start;
  __asm__ __volatile__("mov $_bss_end, %0": "=a"(bss_end));
  __asm__ __volatile__("mov $_text_start, %0": "=a"(text_start));
  return ( bss_end - text_start);
}

typedef struct{
  unsigned int system_memory_size;
  unsigned int system_memory_blocks;
  unsigned int allocated_blocks;
  unsigned int free_blocks;
  unsigned int* memory_map;
  unsigned int memory_map_size;
} PHYSICAL_MEMORY_INFO;

static PHYSICAL_MEMORY_INFO	pm_info;
void printSystemBlocks()
{
  printf("total blocks:%d\n", pm_info.system_memory_blocks);
}
void printFreeBlocks()
{
  printf("total freeblocks:%d\n", pm_info.free_blocks);
}
void printAllocatedBlocks()
{
  printf("total allocated blocks:%d\n", pm_info.allocated_blocks);
}

static inline unsigned int getSystemMemoryBlocks(void)
{
  return pm_info.system_memory_blocks;
}

static inline unsigned int getSystemMemorySize(void)
{
  return pm_info.system_memory_size;
}

static inline void setBit(int bit_num)
{
  pm_info.memory_map[bit_num/32] |= (1<<(bit_num % 32));
}

static inline void clearBit(int bit_num)
{
  pm_info.memory_map[bit_num/32] &= ~(1<<(bit_num % 32));
}

static inline unsigned int testBit(int bit_num)
{
  unsigned int test;
  test = pm_info.memory_map[bit_num/32] & (1<<(bit_num)%32);
  return test;
}
unsigned int testAddress(unsigned int address)
{
  unsigned int ret;
  ret = testBit(address/DEF_MEMORY_BLOCK_SIZE);
  return ret;
}
unsigned int findFirstFreeMemoryBlock(unsigned int *block_number)
{
  unsigned int bitmap_index;
  unsigned int bit_count;
  unsigned int found;

  /* ------------------------------------------------------------------------ */
  /*  iterate each element of the bitmap                                      */
  /* ------------------------------------------------------------------------ */
  for(bitmap_index=0; bitmap_index<pm_info.memory_map_size; bitmap_index++)
  {
      /* -------------------------------------------------------------------- */
      /*  if the element has candidate spaces                                 */
      /* -------------------------------------------------------------------- */
      if(pm_info.memory_map[ bitmap_index ] != 0xFFFFFFFF)
      {
          /* ---------------------------------------------------------------- */
          /*  iterate each bit of the element                                 */
          /* ---------------------------------------------------------------- */
          for(bit_count=0; bit_count<32; bit_count++)
          {
              if(testBit(bitmap_index*32+bit_count) == 0)
              {
                  /* -------------------------------------------------------- */
                  /*  return bitmap index number                              */
                  /* -------------------------------------------------------- */
                  *block_number = bitmap_index * sizeof(unsigned int) * 8 + bit_count;
                  return 1;
              }
          }
      }
  }

  return -1;
}
void kmemset( void *str, unsigned char c, int size )
{
	unsigned char *ptr	= ( unsigned char * )str;
	const unsigned char ch	= ( const unsigned char )c;
	while( size-- )
		*ptr++ = ch;
}

void initFreedMemoryRegion(unsigned int base_address, unsigned int size)
{
  unsigned int block_number;
  int block_size;
  int i;

  block_number = (unsigned int)base_address / DEF_MEMORY_BLOCK_SIZE;
  block_size = size / DEF_MEMORY_BLOCK_SIZE;
  /* ------------------------------------------------------------------------ */
  /*  all blocks of the memory region are freed                               */
  /* ------------------------------------------------------------------------ */
  for(i=0; i<block_size; i++)
  {
    clearBit(block_number);
    block_number++;
    pm_info.allocated_blocks--;
    pm_info.free_blocks++;
  }
}

//カーネルから使えないやつを使用済みにするやつ
void initAllocatedMemoryRegion(unsigned int base_address, unsigned int size)
{
  unsigned int block_number;
  int block_size;
  int i;
  block_number = (unsigned int)base_address / DEF_MEMORY_BLOCK_SIZE;
  block_size = size / DEF_MEMORY_BLOCK_SIZE;
  /* ------------------------------------------------------------------------ */
  /*  all blocks of the memory region are freed                               */
  /* ------------------------------------------------------------------------ */
  for(i=0;i<block_size;i++)
  {
    setBit(block_number);
    block_number++;
    pm_info.allocated_blocks++;
    pm_info.free_blocks--;
  }
}

void* allocSingleMemoryBlock(void)
{
  unsigned int block_number;
  void* physical_address;
  int status;

  if(pm_info.free_blocks<=0)
  {
    return 0;
  }

  status = findFirstFreeMemoryBlock(&block_number);
  if(status != 1)
    return 0;

  setBit(block_number);
  physical_address = (void*)(block_number*DEF_MEMORY_BLOCK_SIZE);
  pm_info.allocated_blocks++;
  pm_info.free_blocks--;
  return physical_address;
}

void freeSingleMemoryBlock(void *physical_address)
{
  unsigned int block_number;
  block_number = (unsigned int)physical_address / DEF_MEMORY_BLOCK_SIZE;
  clearBit(block_number);
  pm_info.allocated_blocks--;
  pm_info.free_blocks++;
}

void initPhysicalMemoryManagement(unsigned int memory_size)
{
  pm_info.system_memory_size = memory_size;
  pm_info.system_memory_blocks = getSystemMemorySize() / DEF_MEMORY_BLOCK_SIZE;
  pm_info.allocated_blocks = getSystemMemoryBlocks();
  pm_info.free_blocks = 0;
  //カーネルの直後を管理用ビットマップとしてつかう
  pm_info.memory_map = (unsigned int*)(DEF_MEMORY_KERNEL_START + getSizeOfKernel());
  pm_info.memory_map_size = getSystemMemoryBlocks() / 32;

  //すべてのブロックをallocatedにする
  kmemset((void*)pm_info.memory_map, 0xFF, pm_info.memory_map_size*8);
}
