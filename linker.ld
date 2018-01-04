/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
    . = 0xC0100000;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
    .text : AT( ADDR( .text ) - 0xC0000000 )
    {
				*(.multiboot)
        *(.text)
    }

    .rodata : AT( ADDR( .rodata ) - 0xC0000000 )
    {
        *(.rodata)
    }

    .data ALIGN ( 0x1000 ): AT( ADDR( .data ) - 0xC0000000 )
    {
        *(.data)
    }

    .bss : AT( ADDR( .bss ) - 0xC0000000 )
    {
        *(COMMON)
        *(.bss)
    }
}
