OUTPUT_FORMAT(elf32-i386)
ENTRY(start)

/* Define the stack size */
STACK_SIZE = 0x4000; /* 16KB stack */

/* Memory regions */
MEMORY
{
    KERNEL_RAM : ORIGIN = 0x00100000, LENGTH = 4M
}

/* Sections */
SECTIONS
{
    . = ORIGIN(KERNEL_RAM);

    .text : {
        KEEP(*(.multiboot_header))
        *(.text*)
    } > KERNEL_RAM

    .rodata : {
        *(.rodata*)
    } > KERNEL_RAM

    .data : {
        *(.data*)
    } > KERNEL_RAM

    .bss : {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    } > KERNEL_RAM

    .stack (NOLOAD) : {
        __stack_bottom = .;
        . = . + STACK_SIZE;
        __stack_top = .;
    } > KERNEL_RAM

    /DISCARD/ : {
        *(.eh_frame)
        *(.comment)
        *(.note*)
    }
}
