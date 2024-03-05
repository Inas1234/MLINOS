KERNEL_LOCATION := ./src/kernel.c
LINKER_LOCATION := ./src/linker/linker.ld
ASM_KERNEL_LOCATION := ./src/kernel.asm
STD_LIB_C_FILES := $(wildcard ./stdlib/*.c)
OUTPUT_DIR := ./output
STD_LIB_O_FILES := $(patsubst ./stdlib/%.c,$(OUTPUT_DIR)/%.o,$(STD_LIB_C_FILES))

$(shell mkdir -p $(OUTPUT_DIR))

default: $(OUTPUT_DIR)/kasm.o $(OUTPUT_DIR)/kc.o $(STD_LIB_O_FILES)
	@echo "Linking kernel..."
	i686-elf-ld -T $(LINKER_LOCATION) -o $(OUTPUT_DIR)/kernel $(OUTPUT_DIR)/kasm.o $(OUTPUT_DIR)/kc.o $(STD_LIB_O_FILES)
	@echo "Done."

$(OUTPUT_DIR)/kasm.o: $(ASM_KERNEL_LOCATION)
	@echo "Compiling kernel assembly..."
	nasm -f elf32 $< -o $@

$(OUTPUT_DIR)/%.o: ./stdlib/%.c
	@echo "Compiling $<..."
	i686-elf-gcc -c $< -o $@ -ffreestanding -nostdlib -nostartfiles -nodefaultlibs

$(OUTPUT_DIR)/kc.o: $(KERNEL_LOCATION)
	@echo "Compiling kernel C code..."
	i686-elf-gcc -c $< -o $@ -ffreestanding -nostdlib -nostartfiles -nodefaultlibs

clean:
	rm -rf $(OUTPUT_DIR)
	@echo "Cleaned up."

run:
	qemu-system-i386 -kernel $(OUTPUT_DIR)/kernel
