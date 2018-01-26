#ifndef COMMON_H
#define COMMON_H
#include <stddef.h>
#include <stdint.h>
extern void printf (const char *format, ...);
void io_outByte(unsigned short address, unsigned char value);
unsigned char io_inByte(unsigned short address);
void kmemset(void *str, unsigned char c, int size);
#define  MAGIC_BREAKPOINT() ({__asm__ __volatile__("xchgw %bx, %bx");})
#endif
