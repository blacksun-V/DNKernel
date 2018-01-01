INCLUDES = -I./include
DIR = /usr/local/i686elfgcc/bin/
default	:
	$(DIR)i686-elf-gcc ${INCLUDES} -c boot.S -o boot.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	$(DIR)i686-elf-gcc ${INCLUDES} -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	$(DIR)i686-elf-gcc -T linker.ld -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc -o myos.elf
	rm -f isofiles/boot/grub/grub.cfg
	cp grub.cfg isofiles/boot/grub/grub.cfg
	rm -f isofiles/boot/myos.elf
	cp myos.elf isofiles/boot/
	rm -f os.iso
	grub-mkrescue -o os.iso isofiles/

clean :
	$(RM) *.o
	$(RM) *.bin

