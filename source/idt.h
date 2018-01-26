#ifndef IDT_H
#define IDT_H

#include "common.h"
#define	DEF_IDT_FLAGS_INTGATE_16BIT		0x06
#define	DEF_IDT_FLAGS_TSKGATE			0x05
#define	DEF_IDT_FLAGS_CALL_GATE			0x0C
#define	DEF_IDT_FLAGS_INTGATE_32BIT		0x0E
#define	DEF_IDT_FLAGS_TRPGATE			0x0F
#define	DEF_IDT_FLAGS_DPL_LV0			0x00
#define	DEF_IDT_FLAGS_DPL_LV1			0x20
#define	DEF_IDT_FLAGS_DPL_LV2			0x40
#define	DEF_IDT_FLAGS_DPL_LV3			0x60
#define	DEF_IDT_FLAGS_PRESENT			0x80

#define	DEF_IDT_INT_SELECTOR			0x08

#define GATE_MAX 256

#define load_idt() ({__asm__ __volatile__("lidt idtr");})

void init_idt(void);


typedef struct{
  unsigned short offsetLow, selector;
  unsigned char reserved, accsessRight;
  unsigned short offsetHigh;
} __attribute__((packed))GATE_DESCRIPTOR;

typedef struct{
  unsigned short size;
  GATE_DESCRIPTOR* base;
} __attribute__((packed))IDTR;

extern void isr0 ();
extern void isr1 ();
extern void isr2 ();
extern void isr3 ();
extern void isr4 ();
extern void isr5 ();
extern void isr6 ();
extern void isr7 ();
extern void isr8 ();
extern void isr9 ();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void irq0 ();
extern void irq1 ();
extern void irq2 ();
extern void irq3 ();
extern void irq4 ();
extern void irq5 ();
extern void irq6 ();
extern void irq7 ();
extern void irq8 ();
extern void irq9 ();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

#endif
