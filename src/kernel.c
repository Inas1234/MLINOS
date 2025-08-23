#include "../stdlib/stdio.h"
#include "pmm.h"
#include "paging.h"
#include "descriptorTables.h"
#include "irq.h"
#include "keyboard.h"
#include "terminal.h"
#include "io.h"

extern volatile unsigned int pit_ticks; // incremented in int0x20_handler
void test_paging(void);                 // from paging.c

static void test_gdt(void) {
    GDTDescriptor gdtr;
    asm volatile("sgdt (%0)" : : "r"(&gdtr));
    kprintf("GDTR base=0x%x, limit=0x%x\n", gdtr.startAddress, gdtr.size);
    unsigned short cs;
    asm volatile("mov %%cs, %0" : "=r"(cs));
    kprintf("CS=0x%x (expect 0x08)\n", cs);
    if ((cs & 0xFFF8) == 0x08) kprintf("GDT selectors: OK\n");
    else                       kprintf("GDT selectors: FAIL\n");
}

static void test_idt(void) {
    IDTDescriptor idtr;
    asm volatile("sidt (%0)" : : "r"(&idtr));
    kprintf("IDTR base=0x%x, limit=0x%x\n", idtr.base, idtr.limit);
    IDTEntry *idt = (IDTEntry *)idtr.base;
    int ok = ((idt[0x20].typeAttr & 0x80) && (idt[0x21].typeAttr & 0x80) && (idt[0x80].typeAttr & 0x80));
    kprintf("IDT present bits 0x20/0x21/0x80: %s\n", ok ? "OK" : "FAIL");
    asm volatile("int $0x80"); // should print from int0x80_handler
}

static void test_irq_pic_masks(void) {
    unsigned char m1 = inb(PIC1_PORT + 1);
    unsigned char m2 = inb(PIC2_PORT + 1);
    kprintf("PIC1 mask=0x%x (expect 0xFC), PIC2 mask=0x%x (expect 0xFF)\n", m1, m2);
    if (m1 == 0xFC && m2 == 0xFF) kprintf("IRQ remap/masks: OK\n");
    else                          kprintf("IRQ remap/masks: WARN\n");
}

static void test_timer_ticks(unsigned int min_ticks) {
    unsigned int start = pit_ticks;
    unsigned int target = start + min_ticks;
    unsigned long spin = 0;
    while (pit_ticks < target && spin < 50000000UL) { spin++; }
    if (pit_ticks >= target) kprintf("PIT test: %u ticks observed. OK\n", pit_ticks - start);
    else                     kprintf("PIT test: no ticks observed (check int0x20_handler/EOI). FAIL\n");
}

void kernel_main() 
{
    clear_screen();
    pmm_init();
    if (pmm_free_blocks_count() == 0) {
        kprintf("Error: No free blocks available.");
        return;
    }
    print_string("Initializing Physical Memory Manager: OK\n");

    init_gdt();
    kprintf("GDT: OK\n");

    init_idt();
    kprintf("IDT: OK\n");

    remap_irq();
    kprintf("IRQs remapping: OK\n");

    init_timer(20);
    kprintf("Timer (20 Hz): OK\n");

    initialize_paging();
    kprintf("Initializing paging: OK.\n");

    // // --- Self-tests before enabling interrupts ---
    // test_gdt();
    // test_idt();
    // test_irq_pic_masks();
    // test_paging();

    // --- Enable devices and interrupts ---
    init_keyboard();
    init_terminal();
    asm volatile("sti");
    kprintf("Welcome to MlinOS.\n");

    // // With interrupts enabled, verify PIT ticks occur
    // test_timer_ticks(5);

    kprintf("> ");
    while (1) {
        char ch = keyboard_get_char();  // Retrieve character from buffer
        if (ch != '\0') {
            if (ch == '\n') {
                terminal_write("\n> ");  // Print new prompt on Enter key
            } else {
                terminal_putchar(ch);    // Print the character
            }
        }
    }
}