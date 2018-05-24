#include "common.h"

void io_outByte(unsigned short address, unsigned char value)
{
  __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(address));
}

unsigned char io_inByte(unsigned short address)
{
  unsigned char ret;
  __asm__ __volatile__("inb %1, %0": "=a"(ret) : "Nd"(address));
  return ret;
}

void kmemset(void *str, unsigned char c, int size)
{
  unsigned char *ptr = (unsigned char *)str;
  const unsigned char ch = (const unsigned char)c;
  while(size--)
    *ptr++ = ch;
}
