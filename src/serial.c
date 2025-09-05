#include "serial.h"

#define COM1 0x3F8

static inline int tx_ready(void) {
    return (inb(COM1 + 5) & 0x20) != 0; 
}

void serial_init(void) {
    outb(COM1 + 1, 0x00); 
    outb(COM1 + 3, 0x80); 
    outb(COM1 + 0, 0x01); 
    outb(COM1 + 1, 0x00); 
    outb(COM1 + 3, 0x03); 
    outb(COM1 + 2, 0xC7); 
    outb(COM1 + 4, 0x0B); 
}

void serial_putc(char c) {
    if (c == '\n') {
        while (!tx_ready()) {}
        outb(COM1, '\r');
    }
    while (!tx_ready()) {}
    outb(COM1, c);
}

void serial_write(const char *s) {
    while (*s) serial_putc(*s++);
}