// idt.c
#include "idt.h"
#include "isr.h"
#include "../stdlib/stdio.h"

struct idt_entry {
    unsigned short base_lo;
    unsigned short sel;        // Kernel segment selector
    unsigned char always0;
    unsigned char flags;       // Type and attributes
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct idt_entry idt_entries[256];
struct idt_ptr idt_ptr;

// Function to set an entry in the IDT
void set_idt_gate(int num, unsigned int base) {
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].sel     = 0x08;          // Kernel code segment
    idt_entries[num].always0 = 0;
    idt_entries[num].flags   = 0x8E;          // Present, ring 0, 32-bit interrupt gate
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;
}

// Loads the IDT
extern void idt_flush(unsigned int);

void idt_install() {
    idt_ptr.limit = sizeof(struct idt_entry) * 256 - 1;
    idt_ptr.base  = (unsigned int)&idt_entries;

    // Clear out the IDT
    memset(&idt_entries, 0, sizeof(struct idt_entry) * 256);

    // ... Set up IDT gates ...
    set_idt_gate(14, (unsigned int)isr14);


    // Load the IDT
    idt_flush((unsigned int)&idt_ptr);
}
