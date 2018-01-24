#define ASM_FILE 1
#include <multiboot2.h>
#define STACK_SIZE 0x4000
#define HEADER_SIZE multiboot_header_end - multiboot_header
#define KERNEL_BASE_ADDRESS 0xC0000000
#define KERNEL_PAGE_NUM (KERNEL_BASE_ADDRESS >> 22)
	.text
	.balign 8
	.extern kernel_entry
  .equ start, (_start-KERNEL_BASE_ADDRESS)
	.global start, _start
  .global multiboot2_info
  .global multiboot2_magic

.balign 8
multiboot_header:
  .long MULTIBOOT2_HEADER_MAGIC
  .long MULTIBOOT_ARCHITECTURE_I386
  .long multiboot_header_end - multiboot_header
  .long -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + HEADER_SIZE)
/*
.balign 8
framebuffer_tag_start:
  .short MULTIBOOT_HEADER_TAG_FRAMEBUFFER
  .short MULTIBOOT_HEADER_TAG_OPTIONAL
  .long framebuffer_tag_end - framebuffer_tag_start
  .long 1024
  .long 768
  .long 32

.balign 8
framebuffer_tag_end:
*/
.balign 8
  .short MULTIBOOT_HEADER_TAG_END
  .short 0
  .long 8

.balign 8
multiboot_header_end:

.balign 8
multiboot_entry:
  //set page directory
  lea     (PageDirectory-KERNEL_BASE_ADDRESS), %ecx
  mov     %ecx, %cr3
  //enable 4MB pages
  mov     %cr4, %ecx
  or      $0x00000010, %ecx
  mov     %ecx, %cr4
  //enable paging
  mov     %cr0, %ecx
  or      $0x80000000, %ecx
  mov     %ecx, %cr0
  //flush pipleline
  lea     HigherHalf, %ecx
  jmp     *%ecx
  HigherHalf:
  // clear first
  // movl $0, (PageDirectory)
  invlpg (0)
  movl $(stack + STACK_SIZE), %esp
  pushl $0
  popf
  movl %ebx, (multiboot2_info)
  movl %eax, (multiboot2_magic)
  call kernel_entry
.comm   stack, STACK_SIZE

.balign 8
start:

.balign 8
_start:
  jmp multiboot_entry

.balign 8
.data
multiboot2_info:
  .long 0
multiboot2_magic:
  .long 0

.align 0x1000
PageDirectory:
  /* ------------------------------------------------------------------------ */
  /* map first 4MB                                                            */
  /* bit7:PS=1 page size is 4MB                                               */
  /* bit1:RW=1 read/write access                                              */
  /* bit0:P =1 page is presence
   0x00000083                                           */
  /* ------------------------------------------------------------------------ */
  .long   0x00000083                            /* first 4MB                  */
      .rept ( KERNEL_PAGE_NUM - 1 )
      .long 0x00000000
  .endr
  /* ------------------------------------------------------------------------ */
  /* map 4MB from 0xC0000000                                                  */
  /* ------------------------------------------------------------------------ */
  .long   0x00000083                          /* for 0xC0000000               */
  .rept ( 1024 - KERNEL_PAGE_NUM - 1 )
      .long 0x00000000
  .endr
