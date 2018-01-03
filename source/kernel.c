#include <stddef.h>
#include <stdint.h>
#include <multiboot2.h>
#include "multiboot.h"

extern void printf (const char *format, ...);
extern void cls (void);
extern void io_hlt(void);
extern void io_sti(void);
extern void io_cli(void);
extern void init_idt(void);
extern void init_gdt(void);
extern void init_pic(void);
extern void init_pit(void);
extern int timercount;
extern unsigned char keydata;
void kernel_entry (){
  unsigned long addr = multiboot2_info;
  struct multiboot_tag *tag;
  unsigned size;
  cls ();
  io_cli();
  printf("\nHello! baby barebone for multiboot2\n");
  printf("magic:%x\n", multiboot2_magic);

  printf("init idt...");
  init_idt();
  printf("[OK]\n");

  printf("init gdt...");
  init_gdt();

  printf("[OK]\n");
  __asm__ __volatile__("int $0x40");
  printf("[*]Back to kernel entry!\n");

  printf("init pic...");
  init_pic();
  printf("[OK]\n");

  printf("init pit...");
  init_pit();
  printf("[OK]\n");
  io_sti();

  while(1){
    if(timercount%100 == 0){
      cls2(0, 1);
      printf("TIMER: %d\n", timercount/100);
    }
    if(keydata!=0x00){
      cls2(1, 1);
      printf("KEY: %x\n", keydata);
      keydata = 0x00;
    }
    io_hlt();
  }
  size = *(unsigned *) addr;
}

  /*
  for (tag = (struct multiboot_tag *) (addr + 8);
      tag->type != MULTIBOOT_TAG_TYPE_END;
      tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
                                      + ((tag->size + 7) & ~7))){
      switch (tag->type)
        {
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
    }
  tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
                                  + ((tag->size + 7) & ~7));
  */
