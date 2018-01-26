
#include "isr.h"
#include "idt.h"

extern void timer_interrupt(void);
extern void ps2keyboard_handler(void);
int int_handler_test();
extern void pageFault(void);

GATE_DESCRIPTOR idt[GATE_MAX];
IDTR idtr;

void setupGateDiscriptor(int iIDT, int offset,
  unsigned short selector, unsigned char ar)
{
  idt[iIDT].offsetLow = (unsigned short)(offset & 0x0000FFFF);
  idt[iIDT].selector = selector;
  idt[iIDT].reserved = 0x00;
  idt[iIDT].accsessRight = ar;
  idt[iIDT].offsetHigh = (unsigned short)((offset & 0xFFFF0000) >> 16);
}

void setupInterruptGate(int iIDT, void *int_handler)
{
  setupGateDiscriptor(iIDT,
    (int)int_handler,
    DEF_IDT_INT_SELECTOR,
    DEF_IDT_FLAGS_PRESENT | DEF_IDT_FLAGS_INTGATE_32BIT | DEF_IDT_FLAGS_DPL_LV0);
}

void init_idt(void)
{
  idtr.size = GATE_MAX * sizeof(GATE_DESCRIPTOR);
  idtr.base = (GATE_DESCRIPTOR*)idt;
  load_idt();

  setupInterruptGate(3, &isr3);
  setupInterruptGate(4, &isr4);
  setupInterruptGate(IRQ0, &irq0);
  setupInterruptGate(IRQ1, &irq1);
}
