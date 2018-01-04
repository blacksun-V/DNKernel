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
        /*put tag*/
        _text_start = .;
		*(.multiboot)
        *(.text)
        _text_end = .;
    }

    .rodata : AT( ADDR( .rodata ) - 0xC0000000 )
    {
        _rodata_start = .;
        *(.rodata)
        _rodata_end = .;
    }

    .data ALIGN ( 0x1000 ): AT( ADDR( .data ) - 0xC0000000 )
    {
        _data_start = .;
        *(.data)
        _data_start = .;
    }

    .bss : AT( ADDR( .bss ) - 0xC0000000 )
    {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        _bss_end = .;
    }
}
