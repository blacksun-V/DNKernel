#define PIC_MASTER_ICW1		0x0020
#define PIC_MASTER_OCW2		0x0020
#define PIC_MASTER_IMR		0x0021
#define PIC_MASTER_ICW2		0x0021
#define PIC_MASTER_ICW3		0x0021
#define PIC_MASTER_ICW4		0x0021
#define PIC_SLAVE_ICW1		0x00a0
#define PIC_SLAVE_OCW2		0x00a0
#define PIC_SLAVE_IMR		0x00a1
#define PIC_SLAVE_ICW2		0x00a1
#define PIC_SLAVE_ICW3		0x00a1
#define PIC_SLAVE_ICW4		0x00a1

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

	io_outByte(PIC_MASTER_IMR,  0xfb  ); /* 11111011 PIC1以外は全て禁止 */
	io_outByte(PIC_SLAVE_IMR,  0xff  ); /* 11111111 全ての割り込みを受け付けない */
	return;
}
