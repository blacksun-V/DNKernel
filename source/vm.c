#include "vm.h"
extern unsigned int allocSingleMemoryBlock(void);
extern void freeSingleMemoryBlock(void *physical_address);
extern void kmemset(void *str, unsigned char c, int size);
extern void io_cli(void);
extern void io_sli(void);
//PTE
inline void setPteFlags(PAGE_TABLE_ENTRY* entry, unsigned long flags)
{
  *entry |= flags;
}

inline void clearPteFlags(PAGE_TABLE_ENTRY* entry, unsigned long flags)
{
  *entry &= ~flags;
}

inline void setPtePageFrameAddress(PAGE_TABLE_ENTRY* entry, unsigned long page_frame_address)
{
  //下位12bitをmaskする
  page_frame_address &= DEF_PTE_FRAME_ADDRESS;
  *entry |= page_frame_address;
}

inline unsigned long getPtePageFrameAddress(PAGE_TABLE_ENTRY* entry)
{
  return (*entry & DEF_PTE_FRAME_ADDRESS);
}

inline int isPtePresent(PAGE_TABLE_ENTRY* entry)
{
	return ((*entry & DEF_PTE_FLAGS_P) == DEF_PTE_FLAGS_P);
}

inline int isPteWritable(PAGE_TABLE_ENTRY* entry)
{
	return ((*entry & DEF_PTE_FLAGS_RW) == DEF_PTE_FLAGS_RW);
}


//PDE
inline void setPdeFlags(PAGE_DIRECTORY_ENTRY* entry, unsigned long flags)
{
  *entry |= flags;
}

inline void clearPdeFlags(PAGE_DIRECTORY_ENTRY* entry, unsigned long flags)
{
  *entry &= ~flags;
}

inline void setPdePageFrameAddress(PAGE_DIRECTORY_ENTRY* entry, unsigned long page_table_address)
{
  page_table_address &= DEF_PDE_PAGE_TABLE_ADDRESS;
  *entry             |= page_table_address;
}

inline unsigned long getPdePageTableAddress(PAGE_DIRECTORY_ENTRY* entry)
{
    return (*entry & DEF_PDE_PAGE_TABLE_ADDRESS);
}

inline int isPdePresent(PAGE_DIRECTORY_ENTRY* entry)
{
    return ((*entry & DEF_PDE_FLAGS_P) == DEF_PDE_FLAGS_P);
}

inline int isPdeWritable(PAGE_DIRECTORY_ENTRY* entry)
{
    return ((*entry & DEF_PDE_FLAGS_P) == DEF_PDE_FLAGS_RW);
}

PAGE_TABLE_ENTRY* allocPage(PAGE_TABLE_ENTRY* entry)
{
  void *physical_address;
  physical_address = allocSingleMemoryBlock();
  if(physical_address == NULL)
  {
    return ((PAGE_TABLE_ENTRY*)physical_address);
  }
  setPtePageFrameAddress(entry, (unsigned long)physical_address);
  setPteFlags(entry, DEF_PTE_FLAGS_P);

  return ((PAGE_TABLE_ENTRY*)physical_address);
}

void freePage(PAGE_TABLE_ENTRY* entry)
{
  void* physical_address;
  physical_address =(void*)getPtePageFrameAddress(entry);
  if(physical_address == NULL)
  {
    return;
  }
  freeSingleMemoryBlock(physical_address);
  clearPteFlags(entry, DEF_PTE_FLAGS_P);
}

static inline unsigned long getPteIndex(unsigned long virtual_address)
{
  unsigned long index;
  index = (virtual_address >> DEF_MM_PTE_INDEX_SHIFT) & DEF_MM_PTE_INDEX_MASK;
  return index;
}

inline PAGE_TABLE_ENTRY* getPTE(PAGE_TABLE* table, unsigned long virtual_address)
{
  PAGE_TABLE_ENTRY* entry;
  if(table == NULL)
  {
    return((PAGE_TABLE_ENTRY*)NULL);
  }
  entry = (PAGE_TABLE_ENTRY*)table;
  return &entry[getPteIndex(virtual_address)];
}

static inline unsigned long getPdeIndex(unsigned long virtual_address)
{
  unsigned long index;
  index = (virtual_address >> DEF_MM_PDE_INDEX_SHIFT) & DEF_MM_PDE_INDEX_MASK;
  return index;
}

inline PAGE_TABLE_ENTRY* getPDE(PAGE_DIRECTORY* directory, unsigned long virtual_address)
{
  PAGE_DIRECTORY_ENTRY* entry;
  if(directory == NULL)
  {
    return((PAGE_DIRECTORY_ENTRY*)NULL);
  }
  entry = (PAGE_DIRECTORY_ENTRY*)directory;
  return &entry[getPdeIndex(virtual_address)];
}

inline void writeCPUCR3(unsigned long value)
{
    __asm__ __volatile__("mov %0, %%cr3" : : "r"(value));
}

inline void switchNewPDE(PAGE_DIRECTORY *directory)
{
  if(directory != NULL){
    writeCPUCR3((unsigned long)directory);
  }
}

static PAGE_DIRECTORY	*current_pd;
inline PAGE_DIRECTORY* getCurrentPageDirectory(void)
{
  return current_pd;
}

inline void invlpg(unsigned long virtual_address)
{
  __asm__ __volatile__("cli");
  __asm__ __volatile__("invlpg %0" : : "m"( virtual_address ) : "memory");
  __asm__ __volatile__("sti");
}

inline void flushTLB(unsigned long virtual_address)
{
  invlpg(virtual_address);
}

int mapPage(unsigned long physical_address, unsigned long virtual_address)
{
  PAGE_DIRECTORY* page_direcory;
  PAGE_DIRECTORY_ENTRY* pde;
  PAGE_TABLE* page_table;
  PAGE_TABLE_ENTRY* pte;

  //get pages
  page_direcory = getCurrentPageDirectory();
  if(page_direcory == NULL)
  {
    return  DEF_MM_ERROR;
  }
  pde = getPDE(page_direcory, virtual_address);

  if(!isPdePresent(pde))
  {
    page_table = (PAGE_TABLE*)allocSingleMemoryBlock();
    if(page_table == NULL)
    {
      return DEF_MM_ERROR;
    }
    //clear pagetable
    kmemset((void*)page_table, 0x00, DEF_MM_PAGE_TABLE_SIZE);

    //create new pde
    setPdeFlags(pde, DEF_PDE_FLAGS_P | DEF_PDE_FLAGS_RW);
    setPdePageFrameAddress(pde, (unsigned long)page_table);
  }else{
    page_table = (PAGE_TABLE*)getPdePageTableAddress(pde);
  }

  //set up pte
  pte = getPTE(page_table, virtual_address);
  setPteFlags(pte, DEF_PTE_FLAGS_P | DEF_PTE_FLAGS_RW);
  setPtePageFrameAddress(pte, physical_address);

  return DEF_MM_OK;
}

inline void pagingOn(void)
{
    io_cli();
    __asm__ __volatile__( "PUSH %eax"           );
    __asm__ __volatile__( "MOV %eax, %cr0 "     );
    __asm__ __volatile__( "OR %eax, $0x80000000" );
    __asm__ __volatile__( "MOV %cr0, %eax"      );
    __asm__ __volatile__( "POP %eax"            );
    io_sli();
}
int initVMManagement(void)
{
  PAGE_DIRECTORY* directory;
  PAGE_DIRECTORY_ENTRY* pde;

  PAGE_TABLE* table_low;
  PAGE_TABLE* table_high;
  PAGE_TABLE_ENTRY* pte;

  int i;
  unsigned long frame;
  unsigned long virtual_address;

  table_low = (PAGE_TABLE*)allocSingleMemoryBlock();
  if(table_low == NULL)
  {
    return DEF_MM_ERROR;
  }

  table_high = (PAGE_TABLE*)allocSingleMemoryBlock();
  if(table_high == NULL)
  {
    return DEF_MM_ERROR;
  }


  //clear page table
  kmemset((void*)table_low, 0x00, DEF_MM_PAGE_TABLE_SIZE);
  kmemset((void*)table_high, 0x00, DEF_MM_PAGE_TABLE_SIZE);

  //set up pages 0x00000000 - 0x003FF000
  frame = 0x00000000;
  virtual_address = 0x00000000;
  for(i=0; i<DEF_MM_NUM_PTE; frame+=DEF_MM_PAGE_SIZE,virtual_address+=DEF_MM_PAGE_SIZE)
  {
    pte = getPTE(table_low, virtual_address);
    setPteFlags(pte, DEF_PTE_FLAGS_P | DEF_PTE_FLAGS_RW);
    setPtePageFrameAddress(pte, frame);
  }

  //set up pages 0xC0000000 - 0xC03FF000
  frame = 0x00100000;
  virtual_address = 0xC0000000;
  for(i=0; i<DEF_MM_NUM_PTE; frame+=DEF_MM_PAGE_SIZE,virtual_address+=DEF_MM_PAGE_SIZE)
  {
    pte = getPTE(table_high, virtual_address);
    setPteFlags(pte, DEF_PTE_FLAGS_P | DEF_PTE_FLAGS_RW);
    setPtePageFrameAddress(pte, frame);
  }

  directory = (PAGE_DIRECTORY*)allocSingleMemoryBlock();
  if(directory == NULL)
  {
    return DEF_MM_ERROR;
  }

  //set up pdes 0x00000000 - 0x003FF000
  pde = getPDE(directory, 0x00000000);
  setPdeFlags(pde, DEF_PDE_FLAGS_P | DEF_PDE_FLAGS_RW);
  setPdePageFrameAddress(pde, (unsigned long)table_low);

  //set up pdes 0xC0000000 - 0xC03FF000
  pde = getPDE(directory, 0x00000000);
  setPdeFlags(pde, DEF_PDE_FLAGS_P | DEF_PDE_FLAGS_RW);
  setPdePageFrameAddress(pde, (unsigned long)table_high);

  switchNewPDE(directory);
  pagingOn();
}
