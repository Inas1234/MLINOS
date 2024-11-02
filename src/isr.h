// isr.h
#ifndef ISR_H
#define ISR_H

typedef struct registers {
    unsigned int ds;                  // Data segment selector
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha
    unsigned int int_no, err_code;    // Interrupt number and error code
    unsigned int eip, cs, eflags, useresp, ss; // Pushed by the processor automatically
} registers_t;

// Function pointer type for interrupt handlers
typedef void (*isr_t)(registers_t *);

extern void isr14();


// ISR installation functions
void isr_install();
void isr_handler(registers_t *r);
void register_interrupt_handler(unsigned char n, isr_t handler);

#endif // ISR_H
