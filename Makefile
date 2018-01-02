SOURCE = ./source
INCLUDES = -I$(SOURCE)/include
DIR = /usr/local/i686elfgcc/bin/
OBJS = $(SOURCE)/kernel.o $(SOURCE)/framebuffer.o $(SOURCE)/screen.o $(SOURCE)/boot.o $(SOURCE)/asmfunc.o $(SOURCE)/idt.o $(SOURCE)/gdt.o $(SOURCE)/pic.o
CC = /usr/local/i686elfgcc/bin/i686-elf-gcc
CFLAGS = -c -std=gnu99 -ffreestanding -O2 -Wall -Wextra $(INCLUDES)
ASMFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -c $(INCLUDES)
LFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib -lgcc -o myos.elf

RM = rm -f
CP = cp
RESCUE = grub-mkrescue
default	:
	$(MAKE) kernel

kernel: $(OBJS) Makefile
	$(CC) $(LFLAGS) $(OBJS)
	$(RM) -f isofiles/boot/grub/grub.cfg
	$(CP) grub.cfg isofiles/boot/grub/grub.cfg
	$(RM) -f isofiles/boot/myos.elf
	$(CP) myos.elf isofiles/boot/
	$(RM) -f os.iso
	$(RESCUE) -o os.iso isofiles/

%.o : %.c Makefile
	$(CC) $*.c -o $*.o $(CFLAGS)

%.o : %.S Makefile
	$(CC) $*.S -o $*.o $(ASMFLAGS)

clean :
	$(RM) *.o
	$(RM) *.bin
