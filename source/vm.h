#ifndef __MEMORY__H
#define __MEMORY__H
/*
==============================================================================

    Description : Page Table Entry


    bit number  label   description
    0           P       0:page is not in memory
                        1:page is present in memory
    1           R/W     0:page is read only
                        1:page is writable
    2           U/S     0:page is kernel( supervisor )mode
                        1:page is user mode.cannnot read or write
                          supervisor pages
    3           PWT     0:cache write back
                        1:cache write through
    4           PCD     0:enable cache
                        1:disable cache
    5           A       0:page has not been accessed
                        1:page has been accessed
    6           D       0:page has not been written to
                        1:page has been written to
    7           PAT     0:pat is not supported
                        1:pat is supported
    8           G       0:not global page
                        1:global page
    9-11        Ignore  available for kernel
    12-31       frame address
==============================================================================
*/
typedef unsigned long PAGE_TABLE_ENTRY;
#define DEF_PTE_FLAGS_P             0x00000001
#define DEF_PTE_FLAGS_RW            0x00000002
#define DEF_PTE_FLAGS_US            0x00000004
#define DEF_PTE_FLAGS_PWT           0x00000008
#define DEF_PTE_FLAGS_PCD           0x00000010
#define DEF_PTE_FLAGS_A             0x00000020
#define DEF_PTE_FLAGS_D             0x00000040
#define DEF_PTE_FLAGS_PAT           0x00000080
#define DEF_PTE_FLAGS_G             0x00000100
#define DEF_PTE_FLAGS_AVAILABLE     0x00000E00
#define DEF_PTE_FRAME_ADDRESS       0xFFFFF000

/*
==============================================================================

    Description : Page Directory Entry


    bit number  label   description
    0           P       0:page group is not in memory
                        1:page gourp is present in memory
    1           R/W     0:page gourp is read only
                        1:page gourp is writable
    2           U/S     0:page gourp is kernel( supervisor )mode
                        1:page gourp is user mode.cannnot read or write
                          supervisor pages
    3           PWT     0:cache write back
                        1:cache write through
    4           PCD     0:enable cache
                        1:disable cache
    5           A       0:page gourp has not been accessed
                        1:page gourp has been accessed
    6           0		reserved

    7           PS      0:page size is 4KB
                        1:page size is 4MB
    8           G       0:not global page gourp
                        1:global page gourp
    9-11        Ignore  available for kernel
    12-31       address of page table
==============================================================================
*/
typedef unsigned long PAGE_DIRECTORY_ENTRY;
#define DEF_PDE_FLAGS_P             0x00000001
#define DEF_PDE_FLAGS_RW            0x00000002
#define DEF_PDE_FLAGS_US            0x00000004
#define DEF_PDE_FLAGS_PWT           0x00000008
#define DEF_PDE_FLAGS_PCD           0x00000010
#define DEF_PDE_FLAGS_A             0x00000020
#define DEF_PDE_FLAGS_RESERVED      0x00000040
#define DEF_PDE_FLAGS_PS            0x00000080
#define DEF_PDE_FLAGS_G             0x00000100
#define DEF_PDE_FLAGS_AVAILABLE     0x00000E00
#define DEF_PDE_PAGE_TABLE_ADDRESS  0xFFFFF000

/*
==================================================================================

	Description : Virtual Memory

==================================================================================
*/
#define	DEF_MM_PAGE_SIZE            4096

#define	DEF_MM_KERNEL_START         0x00010000

#define	DEF_MM_PTE_INDEX_MASK       0x000003FF
#define	DEF_MM_PTE_INDEX_SHIFT      12

#define	DEF_MM_PDE_INDEX_MASK       0x000003FF
#define	DEF_MM_PDE_INDEX_SHIFT      22

typedef PAGE_TABLE_ENTRY PAGE_TABLE;
typedef PAGE_DIRECTORY_ENTRY PAGE_DIRECTORY;

#define	DEF_MM_NUM_PTE              1024
#define	DEF_MM_PAGE_TABLE_SIZE      ( sizeof( PAGE_TABLE_ENTRY ) * DEF_MM_NUM_PTE )
#define	DEF_MM_NUM_PDE              1024
#define	DEF_MM_PAGE_DIRECTORY_SIZE  ( sizeof( PAGE_DIRECTORY_ENTRY ) * DEF_MM_NUM_PDE )
#define NULL 0x00000000
/*
==================================================================================

	Description : Virtual Memory Status

==================================================================================
*/
#define	DEF_MM_OK 0
#define	DEF_MM_ERROR (-1)

PAGE_TABLE_ENTRY* allocPage(PAGE_TABLE_ENTRY* entry);
void freePage(PAGE_TABLE_ENTRY* entry);
int initVMManagement(void);

#endif	/* __MEMORY__H */
