#include <stddef.h>
#include <stdint.h>
#include <multiboot2.h>

struct FrameBufferInfo{
  uint32_t *buffer;
  uint32_t width;
  uint32_t height;
  uint32_t pitch;
  uint8_t bpp;
};
struct FrameBufferInfo fbi;

static void drawCross()
{
  for(uint32_t i = 0; i < fbi.width; i++){
    multiboot_uint32_t *pixel
                        = fbi.buffer + fbi.height/2 * fbi.width + i;
    *pixel = 0x00ff00;
  }
  for(uint32_t i = 0; i< fbi.height*fbi.width; i+=fbi.width){
    multiboot_uint32_t *pixel
                        = fbi.buffer + i + fbi.width/2;
    *pixel = 0x00ff00;
  }
}
