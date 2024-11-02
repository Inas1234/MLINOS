// isr.c
#include "isr.h"
#include "idt.h"
#include "../stdlib/stdio.h"
#include "page_fault.h"

isr_t interrupt_handlers[256];


void isr_install() {
    // Initialize all interrupt handlers to 0
    for (int i = 0; i < 256; i++) {
        interrupt_handlers[i] = 0;
    }

    // ... Set up IDT entries for ISRs ...
    set_idt_gate(14, (unsigned int)isr14);

    // Register the page fault handler
    register_interrupt_handler(14, page_fault_handler);
}

void isr_handler(registers_t *r) {
    if (interrupt_handlers[r->int_no]) {
        isr_t handler = interrupt_handlers[r->int_no];
        handler(r);
    } else {
        kprintf("Unhandled interrupt: %d\n", r->int_no);
    }
}

void register_interrupt_handler(unsigned char n, isr_t handler) {
    interrupt_handlers[n] = handler;
}
