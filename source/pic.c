#include "pic.h"
#include "common.h"
#include "isr.h"

void timer_interrupt(void);
void ps2keyboard_handler(void);

void init_pic(void)
/* PICの初期化 */
{
	io_outByte(PIC_MASTER_IMR, 0xff); /* 全ての割り込みを受け付けない */
	io_outByte(PIC_SLAVE_IMR, 0xff); /* 全ての割り込みを受け付けない */

	io_outByte(PIC_MASTER_ICW1, 0x11); /* エッジトリガモード */
	io_outByte(PIC_MASTER_ICW2, 0x20); /* IRQ0-7は、INT20-27で受ける */
	io_outByte(PIC_MASTER_ICW3, 0x04); /* PIC1はIRQ2にて接続 00100 立ってるbitのところだから2*/
	io_outByte(PIC_MASTER_ICW4, 0x01); /* ノンバッファモード */

	io_outByte(PIC_SLAVE_ICW1, 0x11); /* エッジトリガモード */
	io_outByte(PIC_SLAVE_ICW2, 0x28); /* IRQ8-15は、INT28-2fで受ける */
	io_outByte(PIC_SLAVE_ICW3, 0x02); /* PIC1はIRQ2にて接続 00(IRQ0) 01(IRQ1) 10(IRQ2)*/
	io_outByte(PIC_SLAVE_ICW4, 0x01); /* ノンバッファモード */

	io_outByte(PIC_MASTER_IMR,  0xf8  ); /* 11111000 PIC1 PIT Keyboard */
	io_outByte(PIC_SLAVE_IMR,  0xff  ); /* 11111111 全ての割り込みを受け付けない */

  register_int_handler(IRQ1, &ps2keyboard_handler);
	return;
}

#define PIT_CTRL    0x0043
#define PIT_CNT0    0x0040
int timercount=0;
unsigned char keydata;
void init_pit(void){
  io_outByte(PIT_CTRL, 0x34);
  io_outByte(PIT_CNT0, 0x9c);
  io_outByte(PIT_CNT0, 0x2e);
  register_int_handler(IRQ0, &timer_interrupt);
}

void timer_interrupt(void)
{
  timercount++;
}

void ps2keyboard_handler(void)
{
  keydata = io_inByte(0x60);
}
