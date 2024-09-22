KERNEL_LOCATION := $(wildcard ./src/*.c)
LINKER_LOCATION := ./src/linker/linker.ld
ASM_KERNEL_LOCATION := ./src/kernel.asm
STD_LIB_C_FILES := $(wildcard ./stdlib/*.c)
OUTPUT_DIR := ./output
SRC_O_FILES := $(patsubst ./src/%.c,$(OUTPUT_DIR)/%.o,$(KERNEL_LOCATION))
STD_LIB_O_FILES := $(patsubst ./stdlib/%.c,$(OUTPUT_DIR)/%.o,$(STD_LIB_C_FILES))
ALL_O_FILES := $(SRC_O_FILES) $(STD_LIB_O_FILES)

$(shell mkdir -p $(OUTPUT_DIR))

default: $(OUTPUT_DIR)/kasm.o $(ALL_O_FILES)
	@echo "Linking kernel..."
	i686-elf-ld -T $(LINKER_LOCATION) -o $(OUTPUT_DIR)/kernel $(OUTPUT_DIR)/kasm.o $(ALL_O_FILES)
	@echo "Done."

$(OUTPUT_DIR)/kasm.o: $(ASM_KERNEL_LOCATION)
	@echo "Compiling kernel assembly..."
	nasm -f elf32 $< -o $@

$(OUTPUT_DIR)/%.o: ./stdlib/%.c
	@echo "Compiling $<..."
	i686-elf-gcc -c $< -o $@ -ffreestanding -nostdlib -nostartfiles -nodefaultlibs

$(OUTPUT_DIR)/%.o: ./src/%.c
	@echo "Compiling $<..."
	i686-elf-gcc -c $< -o $@ -ffreestanding -nostdlib -nostartfiles -nodefaultlibs

clean:
	rm -rf $(OUTPUT_DIR)
	@echo "Cleaned up."

run:
	qemu-system-i386 -kernel $(OUTPUT_DIR)/kernel
