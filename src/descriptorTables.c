#include "descriptorTables.h"
#include "../stdlib/stdio.h"
#include "keyboard.h"
#include "io.h"

extern void isr0xUnknown();
extern void isr0x80(void);
extern void isr0x20(void);
extern void isr0x21(void);
extern void isr0x8(void); //double fault
extern void isr0xB(void); //segment not present
extern void isr0xC(void); //stack fault
extern void isr0xD(void); //general protection fault
extern void isr0xE(void); //page fault
extern void isr0xF(void); //unknown interrupt


#define KEYBOARD_DATA_PORT 0x60

// Buffer and index, defined in keyboard.c
extern char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
extern int buffer_index;



GDTDescriptor gdtDesc;
GDTEntry GDT[5];

IDTDescriptor idtDesc;
IDTEntry IDT[256];

int timer = 0;
volatile unsigned int pit_ticks = 0;

void init_gdt()
{
    memset((char*)&GDT[0], 0, sizeof(GDTEntry)); //0x0

    //code 0x8
    GDT[1].limitLow = 0xFFFF;
    GDT[1].baseLow = 0x0;
    GDT[1].baseMiddle = 0x0;
    GDT[1].access = 0x9A; //10011010b
    GDT[1].granularity = 0xCF; //11001111b
    GDT[1].baseHigh = 0;

    //data 0x10
    GDT[2].limitLow = 0xFFFF;
    GDT[2].baseLow = 0x0;
    GDT[2].baseMiddle = 0x0;
    GDT[2].access = 0x92; //10010010b
    GDT[2].granularity = 0xCF; //11001111b
    GDT[2].baseHigh = 0;

    //video memory 0x18
    GDT[3].limitLow = 0xFFFF;
    GDT[3].baseLow = 0x8000;
    GDT[3].baseMiddle = 0x0B;
    GDT[3].access = 0x92; //10010010b
    GDT[3].granularity = 0xCF; //11001111b
    GDT[3].baseHigh = 0;

    //virtual video memory 0x20
    GDT[4].limitLow = 0xFFFF;
    GDT[4].baseLow = 0x6000;
    GDT[4].baseMiddle = 0x50;
    GDT[4].access = 0x92; //10010010b
    GDT[4].granularity = 0xCF; //11001111b
    GDT[4].baseHigh = 0x6;

    gdtDesc.size = sizeof(GDT) - 1;
    gdtDesc.startAddress = (unsigned int) &GDT[0];
    load_gdt(&gdtDesc);
}

void load_gdt(GDTDescriptor *gdtd)
{
    asm volatile ("lgdt (%0)" : : "r"(gdtd));
}

void idt_setEntry(uint number, uint isrAddr, ushort segment, uchar present, uchar dpl, uchar storageSegment, uchar gateType)
{
    IDT[number].offsetLow = isrAddr & 0xFFFF;
    IDT[number].offsetHigh = isrAddr >> 16;
    IDT[number].selector = segment;
    IDT[number].zero = 0;
    IDT[number].typeAttr = ((present & 0x1) << 7) | ((dpl & 0x3) << 5) | ((storageSegment & 0x1) << 4) | (gateType & 0xF);
}

void init_idt()
{
    int i;

    for (i = 0; i < 256; i++)
        idt_setEntry(i, 0, 0, 0, 0, 0, 0xE); //not present

    //for (i = 38; i < 40; i++)
       idt_setEntry(39, (uint)isr0xUnknown, 0x8, 1, 0, 0, 0xE);

    idt_setEntry(0x8, (unsigned int) isr0x8, 0x8, 1, 0, 0, 0xE);
    idt_setEntry(0xB, (unsigned int) isr0xB, 0x8, 1, 0, 0, 0xE);
    idt_setEntry(0xC, (unsigned int) isr0xC, 0x8, 1, 0, 0, 0xE);
    idt_setEntry(0xD, (unsigned int) isr0xD, 0x8, 1, 0, 0, 0xE);
    idt_setEntry(0xF, (unsigned int) isr0xF, 0x8, 1, 0, 0, 0xE);
    idt_setEntry(0xE, (unsigned int) isr0xE, 0x8, 1, 0, 0, 0xE);
    idt_setEntry(0x80, (unsigned int) isr0x80, 0x8, 1, 0, 0, 0xE);
    idt_setEntry(0x20, (uint)isr0x20, 0x8, 1, 0, 0, 0xE); //PIT
    idt_setEntry(0x21, (uint)isr0x21, 0x8, 1, 0, 0, 0xE); //keyboard

    idtDesc.limit = sizeof(IDT) - 1;
    idtDesc.base = (uint) &IDT;
    load_idt(&idtDesc);
}

void load_idt(IDTDescriptor *idtd)
{
    asm volatile ("lidt (%0)" : : "r"(idtd));
}

void int0xUnknown_handler(IntRegisters regs)
{


    kprintf("Unknown interrupt\n");
    kprintf("eax: %x, ebx: %x, ecx: %x, edx: %x\n", regs.eax, regs.ebx, regs.ecx, regs.edx);
    kprintf("esi: %x, edi: %x, esp: %x, ebp: %x\n", regs.esi, regs.edi, regs.esp, regs.ebp);
    kprintf("EFLAGS: %x, cs: %x, eip: %x\n", regs.eflags, regs.cs, regs.eip);


}

void int0x80_handler(IntRegisters regs)
{
    kprintf("INT 0x80 received\n");
}

void int0x20_handler(IntRegisters regs)
{
    pit_ticks++;
    outb(0x20, 0x20); // EOI to master PIC
}

void int0x21_handler(IntRegisters regs) {
    unsigned char scancode = inb(0x60);

    if (!(scancode & 0x80)) {              // key press only
        if (scancode == 0x0E) {            // Backspace scancode
            keyboard_buffer_add('\b');
        } else {
            char ascii = get_ascii_from_scancode(scancode);
            if (ascii) keyboard_buffer_add(ascii);
        }
    }

    outb(0x20, 0x20);                       // EOI
}
void int0x8_handler()
{

    kprintf("#----Error---- Double fault:  pid=0x\n");
}

void int0xB_handler(IntWithErrorRegisters regs)
{

    kprintf("#----Error---- Segment not present:  pid=0x\n");

}

void int0xC_handler()
{
    kprintf("#----Error---- Stack fault:  pid=0x\n");
}

void int0xD_handler()
{
    kprintf("#----Error---- General protection fault:  pid=0x\n");
}

void int0xE_handler(unsigned int faultingAddr)
{

    kprintf("#----Error---- Page fault: 0x%x : pid=0x\n", faultingAddr);
}

void int0xF_handler()
{
    kprintf("#----Error---- Unknown interrupt:  pid=0x\n");
}