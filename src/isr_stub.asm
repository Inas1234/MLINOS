; isr_stub.asm
global isr14
extern isr_handler

section .text

isr14:
    cli                    ; Clear interrupts
    push byte 0            ; Dummy error code
    push byte 14           ; Interrupt number
    jmp isr_common_stub    ; Jump to common ISR code

; Common ISR stub
isr_common_stub:
    pusha                  ; Save general-purpose registers
    push ds                ; Save data segment selector

    mov ax, 0x10           ; Kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push eax               ; Save the current stack pointer in eax
    mov eax, esp           ; Move stack pointer to eax to pass to C handler
    push eax               ; Push eax (which now contains esp) as argument
    call isr_handler       ; Call the ISR handler in C

    pop eax                ; Clean up stack pointer argument
    pop ds                 ; Restore data segment selector
    popa                   ; Restore general-purpose registers
    add esp, 8             ; Clean up stack (int_no and err_code)
    sti                    ; Enable interrupts before returning
    iretd                  ; Return from interrupt
