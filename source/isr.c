#include "common.h"
#include "isr.h"
#include "pic.h"
isr_t int_handlers[256];

void register_int_handler(uint8_t n, void* handler)
{
  int_handlers[n] = handler;
}

void isr_handler(regs_t regs)
{
  if(int_handlers[regs.int_no] != 0)
  {
    isr_t handler = int_handlers[regs.int_no];
    handler(regs);
  }else{
    printf("[*]unhandled int:%d\n", regs.int_no);
    //TODO:
    //printfの実装の問題の可能性
    //nopやmagic breakpointを打たないとスタックが不正な値になっている
    //pop %ebx;mov %bx, %ds
    //ただnopはスタックになんの影響も及ぼさないから調査が必要
    __asm__ __volatile__("nop");
  }
}

void irq_handler(regs_t regs)
{
  if(regs.int_no >= 40)
  {
    io_outByte(PIC_SLAVE_ICW1, 0x20);
  }
  io_outByte(PIC_MASTER_ICW1, 0x20);

  if(int_handlers[regs.int_no] != 0)
  {
    isr_t handler = int_handlers[regs.int_no];
    handler(regs);
  }
}
