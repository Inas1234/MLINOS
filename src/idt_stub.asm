; idt_flush.asm
global idt_flush

section .text
idt_flush:
    mov eax, [esp + 4]   ; Load IDT pointer
    lidt [eax]           ; Load IDT
    sti                  ; Enable interrupts after loading IDT
    ret

