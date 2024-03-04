KERNEL_LOCATION = ./src/kernel.c
LINKER_LOCATION = ./src/linker/linker.ld
ASM_KERNEL_LOCATION = ./src/kernel.asm

default:
	@echo "Compiling kernel..."
	nasm -f elf32 $(ASM_KERNEL_LOCATION) -o kasm.o 
	i686-elf-gcc -c $(KERNEL_LOCATION) -o kc.o -ffreestanding -nostdlib -nostartfiles -nodefaultlibs 
	@echo "Linking kernel..." 
	i686-elf-ld -T $(LINKER_LOCATION) -o kernel kasm.o kc.o
	@echo "Done."

clean:
	rm -f kernel.o kernel.bin
	@echo "Cleaned up."

run:
	qemu-system-i386 -kernel kernel

