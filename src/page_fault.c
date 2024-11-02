// page_fault.c
#include "../stdlib/stdio.h"
#include "page_fault.h"

void page_fault_handler(registers_t *regs) {
    // Get the faulting address from the CR2 register
    unsigned int faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

    // The error code gives us details of what happened.
    int present   = !(regs->err_code & 0x1); // Page not present
    int rw        = regs->err_code & 0x2;    // Write operation?
    int us        = regs->err_code & 0x4;    // Processor was in user-mode?
    int reserved  = regs->err_code & 0x8;    // Overwritten CPU-reserved bits of page entry?
    int id        = regs->err_code & 0x10;   // Caused by an instruction fetch?

    // Output an error message
    kprintf("Page fault at 0x%x (", faulting_address);
    if (present)   { kprintf("Page not present, "); }
    if (rw)        { kprintf("Write operation, "); }
    else           { kprintf("Read operation, "); }
    if (us)        { kprintf("User-mode, "); }
    else           { kprintf("Kernel-mode, "); }
    if (reserved)  { kprintf("Reserved bits overwritten, "); }
    if (id)        { kprintf("Instruction fetch"); }
    kprintf(")\n");

    // For now, halt the system
    kprintf("System Halted.\n");
    while (1) {
        asm volatile("hlt");
    }
}
