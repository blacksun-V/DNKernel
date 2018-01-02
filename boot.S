#define ASM_FILE 1
#include <multiboot2.h>
#define STACK_SIZE 0x4000
#define HEADER_SIZE multiboot_header_end - multiboot_header
	.text
	.balign 8
	.extern kernel_entry
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
  movl $(stack + STACK_SIZE), %esp
  pushl $0
  popf
  movl %ebx, (multiboot2_info)
  movl %eax, (multiboot2_magic)
  call kernel_entry
  hlt
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
