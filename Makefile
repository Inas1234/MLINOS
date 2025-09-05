# Variables for source locations
KERNEL_LOCATION := $(wildcard ./src/*.c)
LINKER_LOCATION := ./src/linker/linker.ld
ASM_KERNEL_LOCATION := ./src/kernel.asm
STD_LIB_C_FILES := $(wildcard ./stdlib/*.c)
OUTPUT_DIR := ./output

# Variables for object files
SRC_O_FILES := $(patsubst ./src/%.c,$(OUTPUT_DIR)/%.o,$(KERNEL_LOCATION))
STD_LIB_O_FILES := $(patsubst ./stdlib/%.c,$(OUTPUT_DIR)/%.o,$(STD_LIB_C_FILES))

# New variables for assembly files (excluding kernel.asm)
ASM_SRC_FILES := $(filter-out $(ASM_KERNEL_LOCATION), $(wildcard ./src/*.asm))
ASM_O_FILES := $(patsubst ./src/%.asm,$(OUTPUT_DIR)/%.o,$(ASM_SRC_FILES))

# Combine all object files except kernel.o (to avoid duplicate linking)
ALL_O_FILES := $(filter-out $(OUTPUT_DIR)/kernel.o, $(SRC_O_FILES)) $(STD_LIB_O_FILES) $(ASM_O_FILES)

# Ensure the output directory exists
$(shell mkdir -p $(OUTPUT_DIR))

# Default target
default: $(OUTPUT_DIR)/kasm.o $(OUTPUT_DIR)/kernel.o $(ALL_O_FILES)
	@echo "Linking kernel..."
	i686-elf-ld -T $(LINKER_LOCATION) -o $(OUTPUT_DIR)/kernel $(OUTPUT_DIR)/kasm.o $(OUTPUT_DIR)/kernel.o $(ALL_O_FILES)
	@echo "Done."

# Rule to compile the kernel assembly file
$(OUTPUT_DIR)/kasm.o: $(ASM_KERNEL_LOCATION)
	@echo "Compiling kernel assembly..."
	nasm -f elf32 $< -o $@

# Rule to compile the main kernel C file
$(OUTPUT_DIR)/kernel.o: ./src/kernel.c
	@echo "Compiling kernel.c..."
	i686-elf-gcc -c $< -o $@ -ffreestanding -nostdlib -nostartfiles -nodefaultlibs

# Rule to compile other assembly files in src (excluding kernel.asm)
$(OUTPUT_DIR)/%.o: ./src/%.asm
	@echo "Compiling $<..."
	nasm -f elf32 $< -o $@

# Rule to compile C source files in stdlib
$(OUTPUT_DIR)/%.o: ./stdlib/%.c
	@echo "Compiling $<..."
	i686-elf-gcc -c $< -o $@ -ffreestanding -nostdlib -nostartfiles -nodefaultlibs

# Rule to compile other C source files in src (excluding kernel.c)
$(OUTPUT_DIR)/%.o: ./src/%.c
	@echo "Compiling $<..."
	i686-elf-gcc -c $< -o $@ -ffreestanding -nostdlib -nostartfiles -nodefaultlibs

# Clean target
clean:
	rm -rf $(OUTPUT_DIR)
	@echo "Cleaned up."

# Run target
run:
	qemu-system-i386 -serial stdio -kernel $(OUTPUT_DIR)/kernel
