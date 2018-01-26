#include <stddef.h>
#include <stdint.h>
#include <multiboot2.h>
#include "multiboot.h"
#include "vm.h"
#include "common.h"
#include "isr.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "phy_mem.h"
#include "vm.h"
#include "screen.h"

void analyze_multiboot_tag(void);
void test_handler(void);

//meminit
uint32_t mm_idx = 0;
uint32_t memsize;
typedef struct{
  char* name;
  uint32_t address;
  uint32_t size;
} MULTIBOOT_MODULE;
MULTIBOOT_MODULE m_modules[10];

uint32_t ua_idx = 0;
typedef struct{
  uint32_t address;
  uint32_t size;
} USABLE_MEMORY;
USABLE_MEMORY usable_areas[10];

void init();
void kernel_entry ()
{
  init();
  while(1){
    if(timercount%100 == 0){
      cls2(0, 1);
      printf("TIMER: %d\n", timercount/100);
    }
    if(keydata!=0x00){
      cls2(1, 2);
      printf("KEY: %x\n", keydata);
      keydata = 0x00;
    }
    io_hlt();
  }
}

void init()
{
  uint32_t eip;
  __asm__ __volatile__("movl $kernel_entry, %0": "=b"(eip));
  cls();
  printf("\n\nNOW I'M AT 0x%x\n", eip);
  io_cli();
  printf("Hello! baby barebone for multiboot2\n");
  printf("magic:%x\n", multiboot2_magic);

  printf("init idt...");
  init_idt();
  printf("[OK]\n");

  printf("init gdt...");
  init_gdt();

  printf("[OK]\n");
  register_int_handler(3, &test_handler);
  __asm__ __volatile__("int $0x3");

  printf("init pic...");
  init_pic();
  printf("[OK]\n");
  analyze_multiboot_tag();
  printf("init pit...");
  init_pit();
  printf("[OK]\n");

  printf("memory size:%x\n", memsize);
  printf("init mem...\n");
  initPhysicalMemoryManagement(memsize);
  //利用可能メモリを全部freeする！！ type1のやつ
  for(int i=0; i<ua_idx; i++){
    printf("0x%x(0x%x) is freed\n", usable_areas[i].address,usable_areas[i].size);
    initFreedMemoryRegion(usable_areas[i].address, usable_areas[i].size);
  }
  for(int i=0; i<mm_idx; i++)
  {
    printf("0x%x(0x%x) is allocated\n", m_modules[i].address,m_modules[i].size);
    initAllocatedMemoryRegion(m_modules[i].address, m_modules[i].size);
  }
  printSystemBlocks();
  printFreeBlocks();
  printAllocatedBlocks();
  printf("[OK]\n");
  printf("pageing setup...");
  int stat = initVMManagement();
  if(stat==0){
    printf("[OK]\n");
  }else{
    printf("[NG]\n");
  }
  io_sti();
  __asm__ __volatile__("int $0x4");
}

void analyze_multiboot_tag(void)
{
  unsigned long addr = multiboot2_info;
  struct multiboot_tag *tag;
  unsigned size;
  size = *(unsigned *) addr;
  for (tag = (struct multiboot_tag *) (addr + 8);
      tag->type != MULTIBOOT_TAG_TYPE_END;
      tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7))){
      //printf ("Tag 0x%x, Size 0x%x\n", tag->type, tag->size);
      switch (tag->type)
      {
        case MULTIBOOT_TAG_TYPE_MODULE:
        {
          /*
          printf ("Module at 0x%x-0x%x. Command line %s\n",
                  ((struct multiboot_tag_module *) tag)->mod_start,
                  ((struct multiboot_tag_module *) tag)->mod_end,
                  ((struct multiboot_tag_module *) tag)->cmdline);*/
          m_modules[mm_idx].address = ((struct multiboot_tag_module *) tag)->mod_start;
          m_modules[mm_idx].name = ((struct multiboot_tag_module *) tag)->cmdline;
          m_modules[mm_idx].size = ((struct multiboot_tag_module *) tag)->mod_end
            - ((struct multiboot_tag_module *) tag)->mod_start;
          mm_idx++;
        }
        break;
        case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
        {
          /*
          printf ("mem_lower = %uKB, mem_upper = %uKB\n",
                  ((struct multiboot_tag_basic_meminfo *) tag)->mem_lower,
                  ((struct multiboot_tag_basic_meminfo *) tag)->mem_upper);*/
          memsize = (uint32_t)(((struct multiboot_tag_basic_meminfo *) tag)->mem_lower) +
                    (uint32_t)(((struct multiboot_tag_basic_meminfo *) tag)->mem_upper);
          memsize *= 1024;
        }
          break;
        case MULTIBOOT_TAG_TYPE_MMAP:
        {
          multiboot_memory_map_t *mmap;
          //printf ("mmap\n");
          for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
                      (multiboot_uint8_t *) mmap
                        < (multiboot_uint8_t *) tag + tag->size;
                      mmap = (multiboot_memory_map_t *)
                        ((unsigned long) mmap
                         + ((struct multiboot_tag_mmap *) tag)->entry_size)){
            /*
            printf (" base_addr = 0x%x%x,"
                           " length = 0x%x%x, type = 0x%x\n",
                           (unsigned) (mmap->addr >> 32),
                           (unsigned) (mmap->addr & 0xffffffff),
                           (unsigned) (mmap->len >> 32),
                           (unsigned) (mmap->len & 0xffffffff),
                           (unsigned) mmap->type);*/
            if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE){
              usable_areas[ua_idx].size = mmap->len & 0xffffffff;
              usable_areas[ua_idx].address = mmap->addr & 0xffffffff;
              ua_idx++;
            }
          }
        }
          break;
        /*
        case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
          {
            multiboot_uint32_t color;
            unsigned i;
            struct multiboot_tag_framebuffer *tagfb
              = (struct multiboot_tag_framebuffer *) tag;
            fb = (void *) (unsigned long) tagfb->common.framebuffer_addr;
            fbi.buffer = (void*) (uint32_t) tagfb->common.framebuffer_addr;
            fbi.width = (uint32_t)tagfb->common.framebuffer_width;
            fbi.height = (uint32_t)tagfb->common.framebuffer_height;
            fbi.pitch = (uint32_t)tagfb->common.framebuffer_pitch;
            fbi.bpp = (uint8_t)tagfb->common.framebuffer_bpp;

            switch (tagfb->common.framebuffer_type){
              case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
              {
                  unsigned best_distance, distance;
                  struct multiboot_color *palette;

                  palette = tagfb->framebuffer_palette;

                  color = 0;
                  best_distance = 4*256*256;

                  for (i = 0; i < tagfb->framebuffer_palette_num_colors; i++){
                    distance = (0xff - palette[i].blue)
                      * (0xff - palette[i].blue)
                      + palette[i].red * palette[i].red
                      + palette[i].green * palette[i].green;
                    if (distance < best_distance){
                      color = i;
                      best_distance = distance;
                    }
                  }
                }
                break;

              case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
                color = ((1 << tagfb->framebuffer_blue_mask_size) - 1)
                  << tagfb->framebuffer_blue_field_position;
                break;

              case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
                color = '\\' | 0x0100;
                break;

              default:
                color = 0xffffffff;
                break;
              }
            color = 0x00ff00;
            for (i = 0; i < fbi.width && i < fbi.height; i++){
              multiboot_uint32_t *pixel = fbi.buffer + fbi.pitch * i + 3 * i;
              *pixel = color;
              drawCross();
            }
          }
        }
    }*/
        }
      }
  tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
                                  + ((tag->size + 7) & ~7));
}

void test_handler(void)
{
  printf("[*]INT0x3 This is test handler!!");
}
