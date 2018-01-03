/*
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Description : Definition for IDT Gate Descriptor Flags

	flags
	bit number	description
	0...4		interruptgate		: 01110b = 32bit descriptor
						: 00110b = 16bit descriptor
			task gate 		: must be 00101b
			trap gate		: 01111b = 32bit descriptor
	5...6		descriptor privedlge	: ring0 = 00b
			level			: ring1 = 01b
						: ring2 = 10b
						: ring3 = 11b
	7		present bit		: Segment is present

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
*/
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

extern void timer_interrupt(void);
extern void ps2keyboard_handler(void);
int int_handler_test();

typedef struct{
  unsigned short offsetLow, selector;
  unsigned char reserved, accsessRight;
  unsigned short offsetHigh;
} __attribute__((packed))GATE_DESCRIPTOR;
GATE_DESCRIPTOR idt[GATE_MAX];

typedef struct{
  unsigned short size;
  GATE_DESCRIPTOR* base;
} __attribute__((packed))IDTR;
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

  setupInterruptGate(0x40, &int_handler_test);
  setupInterruptGate(0x20, &timer_interrupt);
  setupInterruptGate(0x21, &ps2keyboard_handler);
}

extern void printf (const char *format, ...);
int int_handler_test(){
  printf("[*]INT40!!\nThis is just a test interrupt!!\n");
  return 0;
}
