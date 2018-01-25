/*
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	Description :definition for Global descriptor

	flags
	bit number  description
	0...15		segment limit low
	16...31		base address low
	32...39		base address mid
	40		access bit		: unaccessed = 0b
						: accessed = 1b
	41		read/write bit		: read only = 0b
						: read/write = 1b
	42		expansion direction	: up direction = 0b
						: down direction = 1b
	43		segment	type		: data segment = 0b
						: code segment = 1b
	44		descriptor flag		: for system segment = 0b
						: for code, data segment = 1b
	45...46		descriptor privedlge	: ring0 = 00b
			level			: ring1 = 01b
						: ring2 = 10b
						: ring3 = 11b
	47		present bit		: Segment is present
	48...51		segment limit hi
	52		AVL			: free to use by system program
	53		reserved
	54		d/b			: for 16bit = 0b
						: for 32bit = 1b
	55		G			: unit of segment is byte
						: unit of segment is 4K
	56...63		base address hi

_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
*/
#define	NULL_DESCRIPTOR 0
#define	CODE_DESCRIPTOR 1
#define	DATA_DESCRIPTOR 2
#define	CODE_U_DESCRIPTOR 3
#define	DATA_U_DESCRIPTOR 4
#define	TASK_CODE_DESCRIPTOR		3
#define	TASK_DATA_DESCRIPTOR		4
#define	KTSS_DESCRIPTOR			5

/* Null Descriptor		*/
#define	DEF_GDT_NULL_LIMIT		0x0000
#define	DEF_GDT_NULL_BASELO		0x0000
#define	DEF_GDT_NULL_BASEMID		0x00
#define	DEF_GDT_NULL_FLAGS		0x0000
#define	DEF_GDT_NULL_BASEHI		0x00

/* Code Descriptor		*/
#define	DEF_GDT_CODE_LIMIT		0xFFFF
#define	DEF_GDT_CODE_BASELO		0x0000
#define	DEF_GDT_CODE_BASEMID		0x00
#define	DEF_GDT_CODE_FLAGS_BL		0x9A
#define	DEF_GDT_CODE_FLAGS_BH		0xCF
#define	DEF_GDT_CODE_FLAGS		0xCF9A
#define	DEF_GDT_CODE_BASEHI		0x00

/* Data Descriptor		*/
#define	DEF_GDT_DATA_LIMIT		0xFFFF
#define	DEF_GDT_DATA_BASELO		0x0000
#define	DEF_GDT_DATA_BASEMID		0x00
#define	DEF_GDT_DATA_FLAGS		0xCF92
#define	DEF_GDT_DATA_FLAGS_BL		0x92
#define	DEF_GDT_DATA_FLAGS_BH		0xCF
#define	DEF_GDT_DATA_BASEHI		0x00

/* Usermode Code Descriptor*/
#define	DEF_GDT_U_CODE_LIMIT		0xFFFF
#define	DEF_GDT_U_CODE_BASELO		0x0000
#define	DEF_GDT_U_CODE_BASEMID		0x00
#define	DEF_GDT_U_CODE_FLAGS		0xCFFA
#define	DEF_GDT_U_CODE_BASEHI		0x00
/* Usermode Data Descriptor*/
#define	DEF_GDT_U_DATA_LIMIT		0xFFFF
#define	DEF_GDT_U_DATA_BASELO		0x0000
#define	DEF_GDT_U_DATA_BASEMID		0x00
#define	DEF_GDT_U_DATA_FLAGS		0xCFF2
#define	DEF_GDT_U_DATA_BASEHI		0x00

#define GDTMAX 5

inline void load_gdt(void)
{
  __asm__ __volatile__("lgdt gdtr");
  __asm__ __volatile__("mov $0x10, %ax");
  __asm__ __volatile__("mov %ax, %ds");
  __asm__ __volatile__("mov %ax, %es");
  __asm__ __volatile__("mov %ax, %fs");
  __asm__ __volatile__("mov %ax, %gs");
  __asm__ __volatile__("mov %ax, %ss");
  //パイプラインクリア
  __asm__ __volatile__("ljmp $0x08, $_flush_seg");
  __asm__ __volatile__("_flush_seg:");
}

typedef struct{
  unsigned short limitLow,baseLow;
  unsigned char baseMid;
  unsigned short accessRight;
  unsigned char baseHigh;
} __attribute__((packed))SEGMENT_DESCRIPTOR;
SEGMENT_DESCRIPTOR gdt[GDTMAX];

typedef struct{
  unsigned short size;
  SEGMENT_DESCRIPTOR* base;
} __attribute__((packed))GDTR;
GDTR gdtr;

void init_gdt(void)
{
  //NULLディスクリプタ
  gdt[NULL_DESCRIPTOR].limitLow = DEF_GDT_NULL_LIMIT;
  gdt[NULL_DESCRIPTOR].baseLow = DEF_GDT_NULL_BASELO;
  gdt[NULL_DESCRIPTOR].baseMid = DEF_GDT_NULL_BASEMID;
  gdt[NULL_DESCRIPTOR].accessRight = DEF_GDT_NULL_FLAGS;
  gdt[NULL_DESCRIPTOR].baseHigh = DEF_GDT_NULL_BASEHI;

  //コードディスクリプタ
  gdt[CODE_DESCRIPTOR].limitLow = DEF_GDT_CODE_LIMIT;
  gdt[CODE_DESCRIPTOR].baseLow = DEF_GDT_CODE_BASELO;
  gdt[CODE_DESCRIPTOR].baseMid = DEF_GDT_CODE_BASEMID;
  gdt[CODE_DESCRIPTOR].accessRight = DEF_GDT_CODE_FLAGS;
  gdt[CODE_DESCRIPTOR].baseHigh = DEF_GDT_CODE_BASEHI;

  //データディスクリプタ
  gdt[DATA_DESCRIPTOR].limitLow = DEF_GDT_DATA_LIMIT;
  gdt[DATA_DESCRIPTOR].baseLow = DEF_GDT_DATA_BASELO;
  gdt[DATA_DESCRIPTOR].baseMid = DEF_GDT_DATA_BASEMID;
  gdt[DATA_DESCRIPTOR].accessRight = DEF_GDT_DATA_FLAGS;
  gdt[DATA_DESCRIPTOR].baseHigh = DEF_GDT_DATA_BASEHI;

    //コードディスクリプタ
  gdt[CODE_U_DESCRIPTOR].limitLow = DEF_GDT_U_CODE_LIMIT;
  gdt[CODE_U_DESCRIPTOR].baseLow = DEF_GDT_U_CODE_BASELO;
  gdt[CODE_U_DESCRIPTOR].baseMid = DEF_GDT_U_CODE_BASEMID;
  gdt[CODE_U_DESCRIPTOR].accessRight = DEF_GDT_U_CODE_FLAGS;
  gdt[CODE_U_DESCRIPTOR].baseHigh = DEF_GDT_U_CODE_BASEHI;

  //データディスクリプタ
  gdt[DATA_U_DESCRIPTOR].limitLow = DEF_GDT_U_DATA_LIMIT;
  gdt[DATA_U_DESCRIPTOR].baseLow = DEF_GDT_U_DATA_BASELO;
  gdt[DATA_U_DESCRIPTOR].baseMid = DEF_GDT_U_DATA_BASEMID;
  gdt[DATA_U_DESCRIPTOR].accessRight = DEF_GDT_U_DATA_FLAGS;
  gdt[DATA_U_DESCRIPTOR].baseHigh = DEF_GDT_U_DATA_BASEHI;

  gdtr.size = GDTMAX * sizeof(SEGMENT_DESCRIPTOR);
  gdtr.base = (SEGMENT_DESCRIPTOR*)gdt;

  load_gdt();
}
