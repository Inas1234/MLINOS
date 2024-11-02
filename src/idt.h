// idt.h
#ifndef IDT_H
#define IDT_H

void idt_install();
void set_idt_gate(int n, unsigned int handler);

#endif // IDT_H
